#ifndef CLIST_H__
#define CLIST_H__
#pragma once
/*
	          _     _     _
	         | |   (_)   | |
	      ___| |    _ ___| |_
	     / __| |   | / __| __|
	    | (__| |___| \__ \ |_
	     \___\_____/_|___/\__|

	  the missing C list library
	     github.com/qix-/clist
	   (c) 2018-2019, Josh Junon
	         mit license

	 special thanks to rapidstring
	for the header-only inspiration.
*/

#ifdef CLIST_TYPE
#	pragma error("cannot define CLIST_TYPE prior to including clist.h; include clist_type.h instead")
#endif
#ifdef CLIST_NAME
#	pragma error("cannot define CLIST_NAME prior to including clist.h; include clist_type.h instead")
#endif

#include "./clist_type.h" /* defaults to void* and no extra name - thus just the base clist_*() api */

#endif
