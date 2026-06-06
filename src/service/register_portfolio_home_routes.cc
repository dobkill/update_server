#include "service/ApiRoutes.h"

namespace Router {

void ApiRoutes::register_portfolio_home_routes()
{
    auto self = shared_from_this();

    drogon::app().registerHandler(
        "/api/v1/portfolio-home",
        [self](
            const drogon::HttpRequestPtr &req,
            std::function<void(const drogon::HttpResponsePtr &)> &&callback
        )
        {
            (void)req;
            auto json = self->m_productsAction.GetPortfolioHome();
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
            resp->setBody(json.dump());
            callback(resp);
        },
        {drogon::Get}
    );
}

}
