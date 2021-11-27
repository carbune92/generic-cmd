#include "Generic_Cmd.h"

namespace cmd
{
  Generic_Cmd::Generic_Cmd() : m_cid{}, m_sid{}
  {
  }

  Generic_Cmd::Generic_Cmd(def::t_SID sid, def::t_CID cid) : 
    m_cid{cid},
    m_sid{sid}
  {
  }

  def::t_CID Generic_Cmd::getC_ID() const 
  {
    return m_cid;
  }

  def::t_SID Generic_Cmd::getS_ID() const 
  {
    return m_sid;
  }
}
