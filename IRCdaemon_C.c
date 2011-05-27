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

int isDaemon;
char* username;
char* server;
char* channel;

typedef struct
{
  char* channel;
  char* nick;
} irc_ctx_t;

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
	sprintf(query_join,"INSERT INTO joins(user,channel) VALUES('%s', '%s')",origin,params[0]);

	sqlite3_exec(database,query_join,0,0,0);
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	NotUsed=0;
	int i;
	for(i=0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i]: "NULL");
	}
	printf("\n");
fflush(stdout);
	return 0;
}

void event_privmsg(irc_session_t* session, const char* event, 
         const char* origin, const char** params, unsigned int count)
{
	
	fflush(stdout); /* force print*/
	if(strcmp(params[1],"-hallo")==0){
		printf("What do you want to do today?\n");
	}

	if(strcmp(params[1],"-lastlogin")==0){
		char query_last_login[1000];
		sprintf(query_last_login,"SELECT * FROM activity ORDER BY time DESC LIMIT 1");
		sqlite3_exec(database,"",callback,0,0);
	}
	fflush(stdout); /* force print */

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

	/*sql query fpr activity insertion*/
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
		while((opt = getopt(argc, argv, "s:u:c:D::h::"))!= -1) { 
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
	irc_session_t *s;
	irc_ctx_t ctx;

	/* Initialisierung der Callbacks */
	memset (&callbacks, 0, sizeof(callbacks));

	/* Die Callbacks einreichen */
	callbacks.event_connect = event_connect;
	callbacks.event_join    = event_join;
	callbacks.event_channel = event_channel;
	callbacks.event_privmsg = event_privmsg;

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





