#! /usr/local/bin/perl
$RCSid = '$Id: make_langlist.pl,v 1.1.2.2 2006/04/01 15:19:28 amura Exp $';
# This is a part of NG Next Generation
#
# This file is not need perl5, but only perl version 4. Because some platforms
# like Human68k don't have a implementation of perl5.
#
#    making language table
#

sub find_define {
    local ($file) = @_;
    local (@list, %result);
    
    open(INPUT, "<$file") || die "cannot open '$file'";
    local (@list) = grep(/^LANG_DEFINE/, <INPUT>);
    close(INPUT);
    foreach (@list) {
        if (/LANG_DEFINE\s*\(\"([a-zA-Z_ ]+)\"\s*,\s*([a-zA-Z_]+)\)/) {
	    $result{"\l$1"} = $2;
	}
    }
    return %result;
}

sub make_langlist {
    local (%data) = @_;

    print <<__EOS__;
/* Generated automatically from source files of NG lang module */
/*   by $RCSid */
/*
 * This file is the general header file for all parts
 * of the NG display editor.
 */

#ifndef __LANGLIST_H__
#define __LANGLIST_H__

#include "in_code.h"
#include "i_lang.h"

__EOS__

    # make prototypes
    local(%already);
    foreach (keys %data) {
	next if (defined $already{$data{$_}});
	print "LANG_MODULE *$data{$_} _PRO((void));\n";
	$already{$data{$_}} = 1;
    }
    
    print <<__EOS__;

#ifdef DEFINE_LANGLIST
static struct LANGLIST {
    const char *name;
    LANG_MODULE *(*proc)_PRO((void));
} langlist[] = {
__EOS__
    foreach (sort keys %data) {
	print "    { \"$_\", $data{$_}},\n";
    }

    print <<__EOS__;
};
#endif /* DEFINE_LANGLIST */

#endif /* __LANGLIST_H__ */
__EOS__
}

%data = ();
foreach (@ARGV) {
    %data = (%data, &find_define($_));
}
&make_langlist(%data);
