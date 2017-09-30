/* APPLE LOCAL file radar 5435299 */
/* { dg-options "-fnext-runtime -mmacosx-version-min=10.5 -fobjc-abi-version=2" } */
/* { dg-do compile } */

@interface Super  {
  id value;
} 
@property(retain) id value;
@end

@interface Sub : Super @end

@implementation Sub
@synthesize value; /* { dg-error "property" } */
@end


