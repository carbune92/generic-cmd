#ifndef __GENERIC_CMD_H__
#define __GENERIC_CMD_H__

#include "common.h"

namespace cmd
{
  class Generic_Cmd
  {

    protected:
      Generic_Cmd();
      Generic_Cmd(def::t_SID, def::t_CID);
      
    public:
      virtual void execute() = 0;
      virtual def::data_t get_req() const = 0;

      def::t_SID getS_ID() const;
      def::t_CID getC_ID() const;
      virtual ~Generic_Cmd() = default;

    protected:
      def::t_SID m_sid;
      def::t_CID m_cid;
      def::data_t m_req;
  };
}

#endif