/*
 * Copyright (c) 2015, 2016 Apple Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */

#ifndef AGENT_MONITOR_H
#define AGENT_MONITOR_H

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

void process_AgentMonitor();
void process_AgentMonitor_DNS();
void process_AgentMonitor_Proxy();

const void * copy_proxy_information_for_agent_uuid(uuid_t uuid, uint64_t *length);
const void * copy_dns_information_for_agent_uuid(uuid_t uuid, uint64_t *length);

__END_DECLS

#endif /* AGENT_MONITOR_H */
