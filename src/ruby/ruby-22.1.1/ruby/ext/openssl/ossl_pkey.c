/*
 * $Id: ossl_pkey.c,v 1.4.2.1 2004/01/08 12:30:37 gotoyuzo Exp $
 * 'OpenSSL for Ruby' project
 * Copyright (C) 2001-2002  Michal Rokos <m.rokos@sh.cvut.cz>
 * All rights reserved.
 */
/*
 * This program is licenced under the same licence as Ruby.
 * (See the file 'LICENCE'.)
 */
#include "ossl.h"

/*
 * Classes
 */
VALUE mPKey;
VALUE cPKey;
VALUE ePKeyError;
ID id_private_q;

/*
 * callback for generating keys
 */
void
ossl_generate_cb(int p, int n, void *arg)
{
    VALUE ary;

    ary = rb_ary_new2(2);
    rb_ary_store(ary, 0, INT2NUM(p));
    rb_ary_store(ary, 1, INT2NUM(n));

    rb_yield(ary);
}

/*
 * Public
 */
VALUE
ossl_pkey_new(EVP_PKEY *pkey)
{
    if (!pkey) {
	ossl_raise(ePKeyError, "Cannot make new key from NULL.");
    }
    switch (EVP_PKEY_type(pkey->type)) {
#if !defined(OPENSSL_NO_RSA)
    case EVP_PKEY_RSA:
	return ossl_rsa_new(pkey);
#endif
#if !defined(OPENSSL_NO_DSA)
    case EVP_PKEY_DSA:
	return ossl_dsa_new(pkey);
#endif
#if !defined(OPENSSL_NO_DH)
    case EVP_PKEY_DH:
	return ossl_dh_new(pkey);
#endif
    default:
	ossl_raise(ePKeyError, "unsupported key type");
    }
    return Qnil; /* not reached */
}

VALUE
ossl_pkey_new_from_file(VALUE filename)
{
    FILE *fp;
    EVP_PKEY *pkey;

    SafeStringValue(filename);
    if (!(fp = fopen(RSTRING(filename)->ptr, "r"))) {
	ossl_raise(ePKeyError, "%s", strerror(errno));
    }

    pkey = PEM_read_PrivateKey(fp, NULL, ossl_pem_passwd_cb, NULL);
    fclose(fp);
    if (!pkey) {
	ossl_raise(ePKeyError, NULL);
    }

    return ossl_pkey_new(pkey);
}

EVP_PKEY *
GetPKeyPtr(VALUE obj)
{
    EVP_PKEY *pkey;

    SafeGetPKey(obj, pkey);

    return pkey;
}

EVP_PKEY *
GetPrivPKeyPtr(VALUE obj)
{
    EVP_PKEY *pkey;
	
    SafeGetPKey(obj, pkey);
    if (rb_funcall(obj, id_private_q, 0, NULL) != Qtrue) { /* returns Qtrue */
	ossl_raise(rb_eArgError, "Private key is needed.");
    }

    return pkey;
}

EVP_PKEY *
DupPrivPKeyPtr(VALUE obj)
{
    EVP_PKEY *pkey;
	
    SafeGetPKey(obj, pkey);
    if (rb_funcall(obj, id_private_q, 0, NULL) != Qtrue) { /* returns Qtrue */
	ossl_raise(rb_eArgError, "Private key is needed.");
    }
    CRYPTO_add(&pkey->references, 1, CRYPTO_LOCK_EVP_PKEY);

    return pkey;
}

/*
 * Private
 */
static VALUE
ossl_pkey_alloc(VALUE klass)
{
    EVP_PKEY *pkey;
    VALUE obj;

    if (!(pkey = EVP_PKEY_new())) {
	ossl_raise(ePKeyError, NULL);
    }
    WrapPKey(klass, obj, pkey);

    return obj;
}

static VALUE
ossl_pkey_initialize(VALUE self)
{
    if (rb_obj_is_instance_of(self, cPKey)) {
	ossl_raise(rb_eNotImpError, "OpenSSL::PKey::PKey is an abstract class.");
    }
    return self;
}

static VALUE
ossl_pkey_sign(VALUE self, VALUE digest, VALUE data)
{
    EVP_PKEY *pkey;
    EVP_MD_CTX ctx;
    int buf_len;
    VALUE str;

    GetPKey(self, pkey);
    if (rb_funcall(self, id_private_q, 0, NULL) != Qtrue) {
	ossl_raise(rb_eArgError, "Private key is needed.");
    }
    EVP_SignInit(&ctx, GetDigestPtr(digest));
    StringValue(data);
    EVP_SignUpdate(&ctx, RSTRING(data)->ptr, RSTRING(data)->len);
    str = rb_str_new(0, EVP_PKEY_size(pkey)+16);
    if (!EVP_SignFinal(&ctx, RSTRING(str)->ptr, &buf_len, pkey))
	ossl_raise(ePKeyError, NULL);
    assert(buf_len <= RSTRING(str)->len);
    RSTRING(str)->len = buf_len;
    RSTRING(str)->ptr[buf_len] = 0;

    return str;
}

static VALUE
ossl_pkey_verify(VALUE self, VALUE digest, VALUE sig, VALUE data)
{
    EVP_PKEY *pkey;
    EVP_MD_CTX ctx;

    GetPKey(self, pkey);
    EVP_VerifyInit(&ctx, GetDigestPtr(digest));
    StringValue(sig);
    StringValue(data);
    EVP_VerifyUpdate(&ctx, RSTRING(data)->ptr, RSTRING(data)->len);
    switch (EVP_VerifyFinal(&ctx, RSTRING(sig)->ptr, RSTRING(sig)->len, pkey)) {
    case 0:
	return Qfalse;
    case 1:
	return Qtrue;
    default:
	ossl_raise(ePKeyError, NULL);
    }
    return Qnil; /* dummy */
}

/*
 * INIT
 */
void
Init_ossl_pkey()
{
    mPKey = rb_define_module_under(mOSSL, "PKey");
	
    ePKeyError = rb_define_class_under(mPKey, "PKeyError", eOSSLError);

    cPKey = rb_define_class_under(mPKey, "PKey", rb_cObject);
	
    rb_define_alloc_func(cPKey, ossl_pkey_alloc);
    rb_define_method(cPKey, "initialize", ossl_pkey_initialize, 0);

    rb_define_method(cPKey, "sign", ossl_pkey_sign, 2);
    rb_define_method(cPKey, "verify", ossl_pkey_verify, 3);
	
    id_private_q = rb_intern("private?");
	
    /*
     * INIT rsa, dsa
     */
    Init_ossl_rsa();
    Init_ossl_dsa();
    Init_ossl_dh();
}

