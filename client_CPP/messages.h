/**************************************************************************
 * The reference server implementation of the EE122 Project #1
 *
 * Author: Junda Liu (liujd@cs)
 * Author: DK Moon (dkmoon@cs)
 * Author: David Zats (dzats@cs)
**************************************************************************/

#ifndef _messages_h_
#define _messages_h_

#include <assert.h>


/**************************************************************************
  * Message functions for the client
  ************************************************************************/
static void usage(const char *bin)
{
    fprintf(stdout, "! Usage: %s -p <port_number> -s <server_ip>\n");
    fprintf(stdout, "  optionally, -v for verbose output.\n");
}

static inline void show_prompt()
{
  fprintf(stdout, "command> ");
  fflush(stdout);
}

/* Error messages for the client. */
static inline void on_client_connect_failure()
{
  fprintf(stdout,
          "The gate to the tiny world of warcraft is not ready.\n");
}

static inline void on_malformed_message_from_server()
{
  fprintf(stdout, "Meteor is striking the world.\n");
  exit(0);
}

static inline void on_disconnection_from_server()
{
  fprintf(stdout,
          "The gate to the tiny world of warcraft has disappeared.\n");
}

static inline void on_not_visible()
{
  fprintf(stdout, "The target is not visible.\n");
  show_prompt();
}

static inline void on_login_reply(const int error_code)
{
  if (error_code == 0) {
    fprintf(stdout, "Welcome to the tiny world of warcraft.\n");
  } else if (error_code == 1) {
    fprintf(stdout, "A player with the same name is already in the game.\n");
  } else {
    /* This must not happen. The caller of this function must check the
       error code is either 0 or 1 and must perform malformed message
       handling before this function. We simply die here. */
    assert(0);
  }
  show_prompt();
}

static inline void on_move_notify(const char *player_name,
                                  const uint8_t x,
                                  const uint8_t y,
                                  const uint32_t hp,
                                  const uint32_t exp)
{
  fprintf(stdout, "%s: location=(%u,%u), HP=%u, EXP=%u\n",
          player_name, x, y, hp, exp);
  show_prompt();
}

static inline void on_attack_notify(const char *attacker_name,
                                    const char *victim_name,
                                    const int damage,
                                    const int updated_hp)
{
  if (updated_hp <= 0) {
    fprintf(stdout, "%s killed %s\n", attacker_name, victim_name);
  } else {
    fprintf(stdout, "%s damaged %s by %d. %s's HP is now %d\n",
            attacker_name, victim_name, damage, victim_name, updated_hp);
  }
  show_prompt();
}

static inline void on_speak_notify(const char *speaker, const char *message)
{
  fprintf(stdout, "%s: %s\n", speaker, message);
  show_prompt();
}

static inline void on_exit_notify(const char *player_name)
{
  fprintf(stdout, "Player %s has left the tiny world of warcraft.\n",
          player_name);
  show_prompt();
}

static inline void on_invalid_state(const int error_code)
{
  if (error_code == 0) {
    fprintf(stdout, "You must log in first.\n");
  } else if (error_code == 1) {
    fprintf(stdout, "You already logged in.\n");
  } else {
    /* This must not happen. The caller of this function must check the
       error code is either 0 or 1, and must perform malformed message
       handling before this function. We simply die here. */
    assert(0);
  }
  show_prompt();
}
static inline void on_invalid_message() {
    fprintf(stdout, "! Invalid text message.\n");
    show_prompt();
}

static inline void on_invalid_name() {
    fprintf(stdout, "! invalid login name.\n");
    show_prompt();
}

/**************************************************************************
  * Message functions for the server
  ************************************************************************/
/* Error messages for the server. */
static inline const char *message_on_server_port_bind_failure()
{
  return "The gate cannot be opened there.";
}



/**************************************************************************
  * Utility functions
  ************************************************************************/
/* Returns 1 if the name is valid. */
static inline int check_player_name(std::string name)
{
  size_t i;
  /* Must not be null. */
//    EE122_MAX_NAME_LENGTH;
    const char* m = name.c_str();
    if(name.length()>EE122_MAX_NAME_LENGTH)return 0;
  for (i = 0; i < name.length(); ++ i) {
    if (!name[i]) return 0;
    /* Must not contain a non-alphanumeric character. */
    if (!isalnum(name[i])) return 0;
  }
  /* Must be null terminated. */
  return 1;
}

/* Returns if 1 if the text message is valid. */
static inline int check_player_message(std::string message)
{
  size_t i;
  /* Must not be null. */
  const char* m = message.c_str();
  if(message.length()>EE122_MAX_MSG_LENGTH)return 0;
  for (i = 0; i < message.length(); ++ i) {
    if (!m[i]) return 0;
    /* Must not contain a non-printable character. */
    if (!isprint(m[i])) return 0;
  }
  /* Must be null terminated. */
  return 1;
}


#endif /* _messages_h_ */
