/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */

int i = 1st;	/* { dg-error "invalid suffix \"st\" on integer constant" } */
