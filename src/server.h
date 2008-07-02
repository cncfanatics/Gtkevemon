#ifndef SERVER_HEADER
#define SERVER_HEADER

#include <string>

/* Amount of seconds until server is declared as offline. */
#define SERVER_TIMEOUT 5

/* Read N bytes from server to determine status. Don't mess with this. */
#define SERVER_READ_BYTES 24

/*
 * Class for checking if some EVE server is responsive. Also queries
 * the amount of players currently on that server. Only one server
 * at a time can be checked due to non-OO altert behaviour (static stuff).
 *
 * During refresh the is_refreshing() method will return true. This is
 * useful if used in another thread. If players is set to -1, the status
 * has never been checked since creation of the object.
 */
class Server
{
  private:
    static int current_socket;
    static void alarm_expired (int signal);

  private:
    std::string name;
    std::string host;
    uint16_t port;

    bool refreshing;
    bool online;
    int players;

  public:
    Server (std::string const& name, std::string const& host, uint16_t port);

    void refresh (void);

    std::string const& get_name (void) const;
    std::string const& get_host (void) const;
    uint16_t get_port (void) const;
    bool is_online (void) const;
    int get_players (void) const;
    bool is_refreshing (void) const;
};

/* ---------------------------------------------------------------- */

inline std::string const&
Server::get_name (void) const
{
  return this->name;
}

inline std::string const&
Server::get_host (void) const
{
  return this->host;
}

inline uint16_t
Server::get_port (void) const
{
  return this->port;
}

inline bool
Server::is_online (void) const
{
  return this->online;
}

inline int
Server::get_players (void) const
{
  return this->players;
}

inline bool
Server::is_refreshing (void) const
{
  return this->refreshing;
}

#endif /* SERVER_HEADER */
