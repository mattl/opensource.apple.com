=begin
= $RCSfile: ssl.rb,v $ -- Ruby-space definitions that completes C-space funcs for SSL

= Info
  'OpenSSL for Ruby 2' project
  Copyright (C) 2001 GOTOU YUUZOU <gotoyuzo@notwork.org>
  All rights reserved.

= Licence
  This program is licenced under the same licence as Ruby.
  (See the file 'LICENCE'.)

= Version
  $Id: ssl.rb,v 1.1.1.1 2003/10/15 10:11:47 melville Exp $
=end

require 'openssl/buffering'

module OpenSSL
  module SSL
    class SSLSocket
      include Buffering

      def addr
        @io.addr
      end

      def peeraddr
        @io.peeraddr
      end

      def closed?
        @io.closed?
      end
    end
  end
end

