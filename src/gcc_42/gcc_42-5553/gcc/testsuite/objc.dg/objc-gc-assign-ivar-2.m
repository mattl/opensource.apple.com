/* APPLE LOCAL file 5675908 */
/* A write barrier should not be generated if garbage collection is
   disabled.  */
/* { dg-do compile } */
/* { dg-options "-fno-objc-gc -mmacosx-version-min=10.5" } */
@interface Foo
{
  id x;
}
@property(assign) id x;
@end

@implementation Foo
@synthesize x;
@end
/* { dg-final { scan-assembler-not "objc_assign_ivar" } } */
