#include "Support.h"

Range::Range(int s, int e){
    start = s;
    end = e;
}

Range::Range(){
    
}
bool Range::operator==(const Range &r) const{
    return this->start == r.start &&
     this->end == r.end;
}