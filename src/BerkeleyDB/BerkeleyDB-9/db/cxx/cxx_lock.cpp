/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1997-2003
 *	Sleepycat Software.  All rights reserved.
 */

#include "db_config.h"

#ifndef lint
static const char revid[] = "$Id: cxx_lock.cpp,v 1.2 2004/03/30 01:21:24 jtownsen Exp $";
#endif /* not lint */

#include <errno.h>
#include <string.h>

#include "db_cxx.h"
#include "dbinc/cxx_int.h"

////////////////////////////////////////////////////////////////////////
//                                                                    //
//                            DbLock                                  //
//                                                                    //
////////////////////////////////////////////////////////////////////////

DbLock::DbLock(DB_LOCK value)
:	lock_(value)
{
}

DbLock::DbLock()
{
	memset(&lock_, 0, sizeof(DB_LOCK));
}

DbLock::DbLock(const DbLock &that)
:	lock_(that.lock_)
{
}

DbLock &DbLock::operator = (const DbLock &that)
{
	lock_ = that.lock_;
	return (*this);
}
