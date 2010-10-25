#ifndef VECTORUTILS_H
#define VECTORUTILS_H

template <class C> void FreeClear ( C & cntr ) {
    for (typename C::iterator it = cntr.begin(); it != cntr.end(); ++it) {
        delete * it;
    }
     cntr.clear();
}

/* this template deletes a pointer element from a vector and frees the memory */
template <class C, class D> void DeleteElement ( C & cntr, D * element  ) {
    for (typename C::iterator it = cntr.begin(); it != cntr.end(); ++it) {
        if (element == *it) {
            delete *it;
            cntr.erase( it );
            break;
        }
        
    }
}

/* find an element whos child is an element we are deleting (in another func) 
  and modifies the child pointer of found element */
template <class C, class D> void FoldChildren ( C & cntr, D * element  ) {
    for (typename C::iterator it = cntr.begin(); it != cntr.end(); ++it) {
        D * current = *it;
        if (current->child == element) {
            current->child = element->child;
            break;
        }
    }
}

template <class C, class D> D * NextElement ( C & cntr, D * element ) {
    for(typename C::iterator it = cntr.begin(); it != cntr.end(); ++it) {
        D * current = *it;
        if (current->active || element == current) {
            current->active = false;
            if (it == (cntr.end()-1)) {
                // last element was selected
                element = *(cntr.begin());
                element->active = true;
                return element;
            } else {
                element = *(it + 1);
                element->active = true;
                return element;
            }
        }
    }
    element = *(cntr.begin());
    element->active = true;
    return element;
}

template <class C, class D> D * PrevElement ( C & cntr, D * element ) {
    for(typename C::iterator it = cntr.begin(); it != cntr.end(); ++it) {
        D * current = *it;
        if (current->active || element == current) {
            current->active = false;
            if (it == (cntr.begin())) {
                // first element was selected
                element = *(cntr.end()-1);
                element->active = true;
                return element;
            } else {
                element = *(it - 1);
                element->active = true;
                return element;
            }
        }
    }
    element = *(cntr.begin());
    element->active = true;
    return element;
}

#endif
