/* APPLE LOCAL file weak_import on property 6676828 */
/* Ignore weak_import on properties. */
/* Radar 6676828 */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */

@interface foo
@property(nonatomic) int foo __attribute__((weak_import));
@end
