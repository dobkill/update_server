#include "action/ProductsAction.h"

namespace Action {
    json ProductsAction::GetFutureDirections(){
        if (!storage) {
            return json::array();
        }

        return 
           storage->getFutureDirections();
    }
}
