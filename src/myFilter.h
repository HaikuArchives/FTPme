#ifndef MYFILTER_H
#define MYFILTER_H

#ifndef _MESSAGE_FILTER_H
#include <MessageFilter.h>
#endif

class myFilter : public BMessageFilter {
public:
						myFilter();
virtual filter_result	Filter(BMessage *message, BHandler **target);

};
#endif
