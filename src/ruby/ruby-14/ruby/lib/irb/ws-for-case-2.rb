#
#   irb/ws-for-case-2.rb - 
#   	$Release Version: 0.9$
#   	$Revision: 1.1.1.2 $
#   	$Date: 2003/10/15 10:11:49 $
#   	by Keiju ISHITSUKA(keiju@ishitsuka.com)
#
# --
#
#   
#

while true
  IRB::BINDING_QUEUE.push b = binding
end
