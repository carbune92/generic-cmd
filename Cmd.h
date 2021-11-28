#ifndef __CMD_H__
#define __CMD_H__

#include "Generic_Cmd.h"
#include "CmdPolicies.h"
#include <algorithm>
#include <memory>

namespace cmd
{
  template <typename S, typename... P>
  class Cmd {};

  template<typename S, typename P0, typename... P>
  class Cmd<S,P0,P...> : public P0, public Cmd<S,P...>
  {
    public:
    friend Cmd<S>;
    Cmd() {}
    Cmd(def::t_SID sid, def::t_CID cid) : Generic_Cmd(sid, cid) {}

    Cmd(const Cmd<S,P0,P...>& other) : Generic_Cmd{other.m_sid, other.m_cid}
    {
      m_sptr_server = other.m_sptr_server;
      m_callback = other.m_callback;
      m_resp = other.m_resp;
      m_req = other.m_req;
      m_cid = other.m_cid;
      m_sid = other.m_sid;
    }

    Cmd(const Cmd<S>& other) : Generic_Cmd{other.getS_ID(), other.getC_ID()}
    {
      // if (typeid(S) == typeid(Cmd<S>::S))
      // {
        m_sptr_server = other.get_sptr_server();
        m_callback = other.get_callback();
        m_resp = other.get_resp();
        m_req = other.get_req();
        m_cid = other.getC_ID();
        m_sid = other.getS_ID();
      // }
    }

    Cmd(Cmd<S,P0,P...>&& other)
    {
      this->operator=(other);
    }

    // Cmd(Cmd<S>&& other)
    // {
    //   this->operator=(other);
    // }

    Cmd<S,P0,P...>& operator=(const Cmd<S,P0,P...>& other)
    {
      m_sptr_server = other.m_sptr_server;
      m_callback = other.m_callback;
      m_resp = other.m_resp;
      m_req = other.m_req;
      return *this;
    }

    // Cmd<S,P0,P...>& operator=(const Cmd<S>& other)
    // {
    //   m_sptr_server = other.m_sptr_server;
    //   m_callback = other.m_callback;
    //   m_resp = other.m_resp;
    //   m_req = other.m_req;
    //   return *this;
    // }

    Cmd<S,P0,P...>& operator=(Cmd<S,P0,P...>&& other)
    {
      if (this != &other)
      {
        m_resp = def::data_t{};
        m_req = def::data_t{};
        m_cid = 0;
        m_sid = 0;
        std::swap(other.m_sid, this->m_sid);
        std::swap(other.m_cid, this->m_cid);
        std::swap(other.m_req, this->m_req);
        std::swap(other.m_resp, this->m_resp);
        m_sptr_server = other.m_sptr_server;
        m_callback = other.m_callback;
        other.m_sptr_server = nullptr;
        other.m_callback = nullptr;
      }
      return *this;
    }

    // Cmd<S,P0,P...>& operator=(Cmd<S>&& other)
    // {
    //   if (this != &other)
    //   {
    //     m_resp = def::data_t{};
    //     m_req = def::data_t{};
    //     m_cid = 0;
    //     m_sid = 0;
    //     std::swap(other.m_sid, this->m_sid);
    //     std::swap(other.m_cid, this->m_cid);
    //     std::swap(other.m_req, this->m_req);
    //     std::swap(other.m_resp, this->m_resp);
    //     m_sptr_server = other.m_sptr_server;
    //     m_callback = other.m_callback;
    //     other.m_sptr_server = nullptr;
    //     other.m_callback = nullptr;
    //   }
    //   return *this;
    // }

    void init(std::shared_ptr<S> cmdServer, def::Call_t<S> callback, def::data_t params)
    {
      if (cmdServer)
      {
        m_sptr_server = cmdServer;
      }
      m_callback = callback;
      m_req = params;
    }

    virtual void execute() override
    {
      m_resp = std::move( m_callback(*m_sptr_server, m_req) );  
    }

    virtual ~Cmd() = default;

    private:
    std::shared_ptr<S> m_sptr_server;
    def::Call_t<S> m_callback;
    def::data_t m_resp;
    def::data_t m_req;
    
    def::t_SID m_sid;
    def::t_CID m_cid;
  };

  template <typename S>
  class Cmd<S> : virtual public Generic_Cmd
  {
    public:

    // friend Cmd<S,P0,P...>;

    Cmd() {}
    Cmd(def::t_SID  sid, def::t_CID cid) : Generic_Cmd(sid, cid) {}

    Cmd(const Cmd<S>& other)
    {
      m_sptr_server = other.m_sptr_server;
      m_callback = other.m_callback;
      m_resp = other.m_resp;
      m_req = other.m_req;
    }

    Cmd(Cmd<S>&& other)
    {
      this->operator=(other);
    }

    Cmd<S>& operator=(const Cmd<S>& other)
    {
      m_sptr_server = other.m_sptr_server;
      m_callback = other.m_callback;
      m_resp = other.m_resp;
      m_req = other.m_req;
      return *this;
    }

    Cmd<S>& operator=(Cmd<S>&& other)
    {
      if (this != &other)
      {
        m_resp = def::data_t{};
        m_req = def::data_t{};
        m_cid = 0;
        m_sid = 0;
        std::swap(other.m_sid, this->m_sid);
        std::swap(other.m_cid, this->m_cid);
        std::swap(other.m_req, this->m_req);
        std::swap(other.m_resp, this->m_resp);
        m_sptr_server = other.m_sptr_server;
        m_callback = other.m_callback;
        other.m_sptr_server = nullptr;
        other.m_callback = nullptr;
      }
      return *this;
    }
  
    void init(std::shared_ptr<S> cmdServer, def::Call_t<S> callback, def::data_t params)
    {
      if (cmdServer)
      {
        m_sptr_server = cmdServer;
      }
      m_callback = callback;
      m_req = params;
    }

    virtual void execute() override
    {
      m_resp = std::move( m_callback(*m_sptr_server, m_req) );  
    }

    // operator Cmd<S,P0,P...>() const
    // {
    //   return Cmd<S,P0,P...>(*this);
    // }

    virtual ~Cmd() = default;

    std::shared_ptr<S> get_sptr_server() const { return m_sptr_server; }
    def::Call_t<S> get_callback() const { return m_callback; }
    def::data_t get_resp() const { return m_resp; }
    def::data_t get_req() const { return m_req; }
    private:
    std::shared_ptr<S> m_sptr_server;
    def::Call_t<S> m_callback;
    def::data_t m_resp;
    def::data_t m_req;
  };
}

namespace def
{
  template <typename T>
  using EndofCycleCmdPtr_t = std::shared_ptr<cmd::Cmd<T, policies::ExecAtEndOfCycle>>;

  template <typename T>
  using DeallocMemCmdPtr_t = std::shared_ptr<cmd::Cmd<T, policies::DeallocMemPrecond>>;
}

#endif