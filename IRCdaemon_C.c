#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "libircclient.h"

typedef struct
{
  char* channel;
  char* nick;
} irc_ctx_t;

void event_connect (irc_session_t* session, const char* event, 
         const char* origin, const char** params, unsigned int count)
{
  irc_ctx_t* ctx = (irc_ctx_t*) irc_get_ctx(session);
  irc_cmd_join (session, ctx->channel, 0); 
}

void event_join (irc_session_t* session, const char* event, 
         const char* origin, const char** params, unsigned int count)
{
  irc_cmd_msg (session, params[0], "Hi, all!");
}

void event_channel (irc_session_t* session, const char* event, 
         const char* origin, const char** params, unsigned int count)
{
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



