#----------------------------------
# extconf.rb
# $Revision: 1.1.1.1 $
# $Date: 2003/10/15 10:11:48 $
#----------------------------------
require 'mkmf'

def create_win32ole_makefile
  if have_library("ole32") and
     have_library("oleaut32") and
     have_library("uuid") and 
     have_library("user32") and
     have_library("advapi32")
    create_makefile("win32ole")
  end
end

case RUBY_PLATFORM
when /mswin32/
  $CFLAGS += ' /W3'
when /cygwin/, /mingw/
  $defs << '-DNONAMELESSUNION'
end
create_win32ole_makefile
