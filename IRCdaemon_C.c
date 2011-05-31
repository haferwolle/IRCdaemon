#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sqlite3.h>
#include "libircclient.h"

struct sqlite3* database;
irc_session_t *s;
FILE *log_file;



int isDaemon;
char* username;
char* server;
char* channel;

char global_origin[50];

typedef struct
{
  char* channel;
  char* nick;
} irc_ctx_t;


/*this actually doesn't work*/
void shutdown(){
	sqlite3_close(database);
	exit(0);
}

void printHelp()
{
        printf("Usage\n\n") ;
	printf("required:\n") ;
	printf("\t-u irc nick name e.g. -u mynick\n") ;
        printf("\t-s irc server e.g. -s irc.myserver.net\n") ;
	printf("\t-c irc channel e.g. -c '#mychannel'\n") ;

	printf("\noptional:\n");
        printf("\t-D do NOT start as a daemon\n") ;
        printf("\t-h get help\n") ;
}


/* SQL Callbacks */
static int SQLCallback(void *NotUsed, int argc, char **argv, char **azColName) 
{ 

  int i; 
  for(i=0; i<argc; i++)
  { 
	
	char str_sql[500];
	
	sprintf(str_sql,"%s : %s\n", azColName[i], argv[i] ? argv[i] : "NULL");  
	
	irc_cmd_msg(s, global_origin, str_sql);
  } 

  printf("\n"); 
  return 0; 
}

static int SQLCallback_File(void *NotUsed, int argc, char **argv, char **azColName) 
{ 


  int i; 
  
  for(i=0; i<argc; i++)
  { 
	
	fprintf(log_file, "%s : %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	printf("%s : %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	
  } 

  printf("\n");
  return 0; 
}


/*Event Callbacks*/
void event_connect (irc_session_t* session, const char* event, 
         const char* origin, const char** params, unsigned int count)
{
  irc_ctx_t* ctx = (irc_ctx_t*) irc_get_ctx(session);
  irc_cmd_join (session, ctx->channel, 0); 
}

void event_join (irc_session_t* session, const char* event, 
         const char* origin, const char** params, unsigned int count)
{

	irc_cmd_msg (session, params[0], "Hi all, Big Brother is logging you!");

	char query_join[2048];
	sprintf(query_join,"INSERT INTO joins(user,channel,time) VALUES('%s', '%s',datetime('now'))",origin,params[0]);

	sqlite3_exec(database,query_join,0,0,0);

	if(isDaemon==0)
	{
		printf("Der User: '%s', hat sich soeben eingeloggt\n",origin);
	}
}




void event_part (irc_session_t* session, const char* event, 
         const char* origin, const char** params, unsigned int count)
{

	char query_quit[500];

	printf("Der User: '%s', hat sich soeben ausgeloggt\n", origin);
	
	sprintf(query_quit, "INSERT INTO left (user, channel, reason, time) VALUES('%s', '%s', '%s', datetime('now'))",origin, params[0], params[1]);
	sqlite3_exec(database, query_quit,0,0,0);
	
}

void event_privmsg (irc_session_t* session, const char* event, 
         const char* origin, const char** params, unsigned int count) 
{

	printf("'%s' sagte '%s' zu dir\n", origin, params[1]);
	
	if (strcmp(params[1],"-hallo")==0)
	{
		printf("Hab die Ehre! Gib -help ein für weitere Kommandos!\n");
		irc_cmd_msg(session, origin, "Ich reagiere auf private Nachrichten");
	}
	
	if (strcmp(params[1],"-getLatest")==0)
	{
		strcpy(global_origin, origin);
		
		sqlite3_exec(database, "SELECT * FROM joins ORDER BY time DESC LIMIT 1",SQLCallback,0,0);	
		
		
		
		printf("---%s---\n",global_origin);
		
		printf("Select wurde ausgeführt\n");

	}
	
	if (strcmp(params[1],"-help")==0)
	{
	
		irc_cmd_msg(session, origin, "-hallo : Statusmeldung zurück");
		irc_cmd_msg(session, origin, "-getLatest : gib letzten User an, der den Chat betreten hat");
		irc_cmd_msg(session, origin, "-help: genau das Hier");
		irc_cmd_msg(session, origin, "-logfile: Erstellt eine Log-Datei");


	}
	
	if (strcmp(params[1],"-logfile")==0)
	{
		log_file = fopen("log.txt", "w");
		sqlite3_exec(database, "Select * From joins",SQLCallback_File,0,0);	
		fclose(log_file);
	}
	

	
}

void event_channel (irc_session_t* session, const char* event, 
         const char* origin, const char** params, unsigned int count)
{

	/*if not a daemon, print channel activity to stdout*/
	if(isDaemon==0)
	{
		printf ("'%s' hat im Channel %s gesagt: %s\n", 
		origin ? origin : "irgendwer", 
		params[0], params[1]);
	}	

	/*sql query for activity insertion*/
	char query_activity[2048];
	sprintf
	(
		query_activity,
		"INSERT INTO activity (user,channel,message,time) VALUES('%s', '%s','%s',datetime('now'))",
		origin,params[0],params[1]
	);

	/*execute query*/
	sqlite3_exec(database,query_activity,0,0,0);
}

/*Aus Vorlesung 20110411*/
static void daemonize()
{
	pid_t pid, sid;

	if (getppid() == 1) return;

	pid = fork();
	if ( pid < 0 ) exit (1);

	/* Wenn ein Kindprozess erzeugt wurde, kann der 
	Eltern Prozess sich beenden 
	*/

	if (pid > 0) exit (0);

	/* Neues file mode mask */
	umask(0);

	/* Neue SID Umgebung */
	sid = setsid();
	if (sid < 0) exit(1);

	/* Aendern des Arbeitsverzeichnisses */
	if ((chdir("/") < 0)) exit(1); 

	/* Aendern der default Filedescriptoren */
	freopen("/dev/null", "r", stdin);
	freopen("/dev/null", "w", stdout);
	freopen("/dev/null", "w", stderr);
}



int main(int argc, char** argv)
{
  	isDaemon=1;


	if (argc > 1){

		int opt=0;

		//long options like --help
		while ((opt = getopt(argc, argv, "s:u:c:D::h::")) != -1) 
		{
			switch(opt) { 
				case 's': server=optarg; 
					break;
				case 'u': username=optarg; 
					break;
				case 'c': channel=optarg; 
					break;

				case 'D': isDaemon=0; 
					break;
				case 'h': printHelp();
					return 0;
					break;

				
			}
	
		}
	}	
	

	//required params are not provided
	if(username==0 || channel==0 || server==0)
	{
		printf("Wrong or missing arguments!\n\n") ;
		printHelp();
		return 0;
	}

	if(isDaemon==1){
	  daemonize();
	}

	/*open database connection*/
	sqlite3_open("logging.db",&database);

	/*IRC CONNECTION*/
	irc_callbacks_t callbacks;
	irc_ctx_t ctx;

	/* Initialisierung der Callbacks */
	memset (&callbacks, 0, sizeof(callbacks));

	/* Die Callbacks einreichen */
	callbacks.event_connect = event_connect;
	callbacks.event_join    = event_join;
	callbacks.event_channel = event_channel;
	callbacks.event_privmsg = event_privmsg;
	callbacks.event_part    = event_part;

	ctx.channel = channel;
	ctx.nick    = username;

	s = irc_create_session(&callbacks);
	if (!s)
	{
	printf("Konnte die Sitzung nicht konfigurieren...\n");
	return 1;
	}

	irc_set_ctx(s, &ctx);
	irc_option_set(s, LIBIRC_OPTION_STRIPNICKS);

	/* Verbindung aufbauen */
	if ( irc_connect(s, server, 6667, 0, username, 0, 0))
	{
	printf("Konnte keine Verbindung zum Server aufbauen...\n");
	return 1;
	}

	irc_run(s);

	return 0;
}





