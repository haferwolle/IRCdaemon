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

typedef struct
{
  char* channel;
  char* nick;
} irc_ctx_t;

void shutdown(){
	sqlite3_close(database);
	exit(0);
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

void event_channel (irc_session_t* session, const char* event, 
         const char* origin, const char** params, unsigned int count)
{
/*
	printf ("'%s' hat im Channel %s gesagt: %s\n", 
	origin ? origin : "irgendwer", 
	params[0], params[1]);
*/

	if(params[1]=="!!!!!"){
		shutdown();
	}	

	char query_activity[2048];
	sprintf
	(
		query_activity,
		"INSERT INTO activity (user,channel,message,time) VALUES('%s', '%s','%s',datetime('now'))",
		origin,params[0],params[1]
	);

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
  sqlite3_open("logging.db",&database);
  
  daemonize();

  irc_callbacks_t callbacks;
  irc_session_t *s;
  irc_ctx_t ctx;

  if ( argc != 4)
  {
    printf ("Usage: %s <server> <nick> <channel>\n", argv[0]);
    return 1;
  }


  /* Initialisierung der Callbacks */
  memset (&callbacks, 0, sizeof(callbacks));

  /* Die Callbacks einreichen */
  callbacks.event_connect = event_connect;
  callbacks.event_join    = event_join;
  callbacks.event_channel = event_channel;

  
  ctx.channel = argv[3];
  ctx.nick    = argv[2];

  s = irc_create_session(&callbacks);
  if (!s)
  {
    printf("Konnte die Sitzung nicht konfigurieren...\n");
    return 1;
  }


  irc_set_ctx(s, &ctx);
  irc_option_set(s, LIBIRC_OPTION_STRIPNICKS);

  /* Verbindung aufbauen */
  if ( irc_connect(s, argv[1], 6667, 0, argv[2], 0, 0))
  {
    printf("Konnte keine Verbindung zum Server aufbauen...\n");
    return 1;
  }

  irc_run(s);

  
  return 0;

}





