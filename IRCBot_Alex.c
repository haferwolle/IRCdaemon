#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sqlite3.h>
#include "libircclient.h"

struct sqlite3* Database;
irc_session_t *s;
FILE *log_file;

int isDaemon;
char* username;
char* server;
char* channel;

static char global_origin[50];

typedef struct
{
  char* channel;
  char* nick;
} irc_ctx_t;



void shutdown(){
	sqlite3_close(Database);
	
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
  fflush(stdout);
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
  
  
  irc_cmd_msg (session, params[0], "Hi, Mein Bot FUNZT!!");
  
  char str_join[200];
  
  sprintf(str_join, "INSERT INTO join_irc (Channel, User, Joindate, Jointime) Values ('%s', '%s', date('now'), time('now'))",params[0], origin);
  
  sqlite3_exec(Database, str_join,0,0,0);
  
  
  printf("Der User: '%s', hat sich soeben eingeloggt\n",origin);

}




void event_part (irc_session_t* session, const char* event, 
         const char* origin, const char** params, unsigned int count)
{

	char str_quit[500];

	printf("Der User: '%s', hat sich soeben ausgeloggt\n", origin);
	
	sprintf(str_quit, "INSERT INTO part_irc (user, channel, reason, partdate, parttime) VALUES('%s', '%s', '%s', date('now'), time('now'))",origin, params[0], params[1]);
	sqlite3_exec(Database, str_quit,0,0,0);
	
}

void event_privmsg (irc_session_t* session, const char* event, 
         const char* origin, const char** params, unsigned int count) 
{

	printf("'%s' sagte '%s' zu dir\n", origin, params[1]);
	
	if (strcmp(params[1],"-hallo")==0)
	{
		printf("Ich reagiere auf Private Messages\n");
		irc_cmd_msg(session, origin, "Ich reagiere auf private Nachrichten");
		
	}
	
	if (strcmp(params[1],"-login")==0)
	{
		strcpy(global_origin, origin);
		
		sqlite3_exec(Database, "Select * From join_irc Order by rowid desc limit 1",SQLCallback,0,0);	
		
		
		printf(global_origin);
		
		fflush(stdout);

	}
	
	if (strcmp(params[1],"-help")==0)
	{
	
		irc_cmd_msg(session, origin, "-hallo : Statusmeldung zurück");
		irc_cmd_msg(session, origin, "-login : die logintable auslesen");
		irc_cmd_msg(session, origin, "-help: genau das Hier");
		irc_cmd_msg(session, origin, "-logfile: Erstellt die Log Datei");


	}
	
	if (strcmp(params[1],"-logfile")==0)
	{
		log_file = fopen("log.txt", "w");
		sqlite3_exec(Database, "Select * From join_irc",SQLCallback_File,0,0);
		fclose(log_file);		
		
	}
	

	
}

void event_channel (irc_session_t* session, const char* event, 
         const char* origin, const char** params, unsigned int count)

{
	char str_insert[10000];
	
	sprintf(str_insert, "INSERT INTO log_irc (User, Channel, Message, InsertDate, InsertTime) Values ('%s', '%s', '%s', date('now'), time('now'))", origin, params[0], params[1]);
		
	sqlite3_exec(Database, str_insert,0,0,0);

    printf ("'%s' hat im Channel %s gesagt: %s\n", 
    origin ? origin : "irgendwer", 
    params[0], params[1]);
	
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
	sqlite3_open("knowledge.db",&Database);

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
	callbacks.event_part 	= event_part;

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





