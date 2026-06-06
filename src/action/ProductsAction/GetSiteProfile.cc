#include "action/ProductsAction.h"

namespace Action {
    json ProductsAction::GetSiteProfile(){
        if (!storage) {
            return {
                {"site_name", "YXX Works"},
                {"subtitle", "产品、插件与创作实验"},
                {"github_url", ""},
                {"email", ""}
            };
        }

        return 
       storage->getSiteProfile();
    }
}
