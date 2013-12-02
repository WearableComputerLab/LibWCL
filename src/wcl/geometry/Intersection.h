
#ifndef WCL_INTERSECTION_H
#define WCL_INTERSECTION_H

#include <iostream>

namespace wcl {
    
    struct Intersection {

        public:
            /*
             * Intersection points between two geometrys are either:
             *    Non existant
             *    Existant (single point)
             *    Existant (every point, ie co-planar). 
             * This isnt' the best naming scheme, but I dont' know the correct
             * math terms. 
             */
            enum Intersects { YES, NO, IS_SAME }; 
            Intersects intersects; // Our local intersection storage.
            /*
             * If we have a single intesection point, store it here
             */
            wcl::Vector point;

            bool operator==(const Intersection& ip) const { 
                if ( this->intersects == NO || this->intersects == IS_SAME ) 
                    return (intersects == ip.intersects);
                else
                    return ((intersects == ip.intersects) && (point == ip.point));
            };

            bool operator!=(const Intersection& ip) const {
                if ( this->intersects == NO || this->intersects == IS_SAME ) 
                    return (intersects != ip.intersects);
                else
                    return ((intersects != ip.intersects) || (point != ip.point));
            };
    };
}

inline std::ostream& operator<< (std::ostream& os, const wcl::Intersection& i) {
    return os << "<Intersection: " << i.intersects << " at " << i.point << ">";
}

#endif
