# parsedate.rb: Written by Tadayoshi Funaba 2001, 2002
# $Id: parsedate.rb,v 1.1.1.3 2003/10/15 10:11:48 melville Exp $

require 'date/format'

module ParseDate

  def parsedate(str, comp=false)
    Date._parse(str, comp).
      values_at(:year, :mon, :mday, :hour, :min, :sec, :zone, :wday)
  end

  module_function :parsedate

end
