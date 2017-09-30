/* APPLE LOCAL file 4695109 - modified for radar 6255913 */
/* Check for generation of protocol meta-data */
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-abi-version=2" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@protocol Proto1
@end

@protocol Proto2
@end

@interface Super <Proto1, Proto2> { id isa; } @end
@implementation Super @end
/* { dg-final { if [istarget i?86-*-darwin* ] { scan-assembler "L_OBJC_PROTOCOL_\\\$_Proto1:" } } } */
/* { dg-final { if [istarget powerpc*-*-darwin* ] { scan-assembler "L_OBJC_PROTOCOL_\\\$_Proto1:" } } } */
/* { dg-final { if [istarget arm*-*-darwin* ] { scan-assembler "l_OBJC_PROTOCOL_\\\$_Proto1:" } } } */
/* { dg-final { if [istarget i?86-*-darwin* ] { scan-assembler "L_OBJC_PROTOCOL_\\\$_Proto2:" } } } */
/* { dg-final { if [istarget powerpc*-*-darwin* ] { scan-assembler "L_OBJC_PROTOCOL_\\\$_Proto2:" } } } */
/* { dg-final { if [istarget arm*-*-darwin* ] { scan-assembler "l_OBJC_PROTOCOL_\\\$_Proto2:" } } } */
