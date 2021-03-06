/*
 * Copyright (c) 2002-2004 Apple Computer, Inc. All Rights Reserved.
 * 
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.
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

//
// Identity.cpp - Working with Identities
//
#include <security_keychain/Identity.h>

#include <security_cdsa_utilities/KeySchema.h>
#include <security_keychain/KCCursor.h>

using namespace KeychainCore;

Identity::Identity(const SecPointer<KeyItem> &privateKey,
		const SecPointer<Certificate> &certificate) :
	mPrivateKey(privateKey),
	mCertificate(certificate)
{
}

Identity::Identity(const StorageManager::KeychainList &keychains, const SecPointer<Certificate> &certificate) :
	mCertificate(certificate)
{
	// Find a key whose label matches the publicKeyHash of the public key in the certificate.
	KCCursor keyCursor(keychains, CSSM_DL_DB_RECORD_PRIVATE_KEY, NULL);
	keyCursor->add(CSSM_DB_EQUAL, KeySchema::Label, certificate->publicKeyHash());

	Item key;
	if (!keyCursor->next(key))
		MacOSError::throwMe(errSecItemNotFound);

	SecPointer<KeyItem> keyItem(static_cast<KeyItem *>(&*key));
	mPrivateKey = keyItem;
}

Identity::~Identity() throw()
{
}

SecPointer<KeyItem>
Identity::privateKey() const
{
	return mPrivateKey;
}

SecPointer<Certificate>
Identity::certificate() const
{
	return mCertificate;
}
