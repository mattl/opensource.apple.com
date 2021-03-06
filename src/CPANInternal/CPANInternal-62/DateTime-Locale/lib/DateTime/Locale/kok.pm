###########################################################################
#
# This file is auto-generated by the Perl DateTime Suite time locale
# generator (0.02).  This code generator comes with the
# DateTime::Locale distribution in the tools/ directory, and is called
# generate_from_icu.
#
# This file as generated from the ICU XML locale data.  See the
# LICENSE.icu file included in this distribution for license details.
#
# This file was generated from the source file kok.xml.
# The source file version number was 1.2, generated on
# 2004-08-27.
#
# Do not edit this file directly.
#
###########################################################################

package DateTime::Locale::kok;

use strict;

BEGIN
{
    if ( $] >= 5.006 )
    {
        require utf8; utf8->import;
    }
}

use DateTime::Locale::root;

@DateTime::Locale::kok::ISA = qw(DateTime::Locale::root);

my @day_names = (
"सोमवार",
"मंगळार",
"बुधवार",
"गुरुवार",
"शुक्रवार",
"शनिवार",
"आदित्यवार",
);

my @day_abbreviations = (
"सोम",
"मंगळ",
"बुध",
"गुरु",
"शुक्र",
"शनि",
"रवि",
);

my @month_names = (
"जानेवारी",
"फेब्रुवारी",
"मार्च",
"एप्रिल",
"मे",
"जून",
"जुलै",
"ओगस्ट",
"सेप्टेंबर",
"ओक्टोबर",
"नोव्हेंबर",
"डिसेंबर",
);

my @month_abbreviations = (
"जानेवारी",
"फेबृवारी",
"मार्च",
"एप्रिल",
"मे",
"जून",
"जुलै",
"ओगस्ट",
"सेप्टेंबर",
"ओक्टोबर",
"नोव्हेंबर",
"डिसेंबर",
);

my @am_pms = (
"म\.पू\.",
"म\.नं\.",
);

my @eras = (
"क्रिस्तपूर्व",
"क्रिस्तशखा",
);



sub day_names                      { \@day_names }
sub day_abbreviations              { \@day_abbreviations }
sub month_names                    { \@month_names }
sub month_abbreviations            { \@month_abbreviations }
sub am_pms                         { \@am_pms }
sub eras                           { \@eras }



1;

