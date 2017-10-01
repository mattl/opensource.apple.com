# just for compatibility; requiring "md5" is obsoleted
#
# $RoughId: md5.rb,v 1.4 2001/07/13 15:38:27 knu Exp $
# $Id: md5.rb,v 1.1.1.2 2003/10/15 10:11:47 melville Exp $

require 'digest/md5'

MD5 = Digest::MD5

class MD5
  def self.md5(*args)
    new(*args)
  end
end
