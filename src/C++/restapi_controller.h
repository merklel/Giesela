//
// Created by lukas on 10.06.23.
//

#ifndef GISELA_RESTAPI_CONTROLLER_H
#define GISELA_RESTAPI_CONTROLLER_H

#include <future>
#include "DTOs.hpp"
#include "restapi.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin Codegen
#include "gisela.hpp"

/**
 * Sample Api Controller.
 */
class MyController : public oatpp::web::server::api::ApiController {
public:
    Gisela *gisela;
    /**
     * Constructor with object mapper.
     * @param objectMapper - default object mapper used to serialize/deserialize DTOs.
     */
//    MyController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
//    : oatpp::web::server::api::ApiController(objectMapper)
//    {}
    MyController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper), Gisela *gisela = NULL)
    : oatpp::web::server::api::ApiController(objectMapper)
    {
        this->gisela = gisela;
    }
public:

    ENDPOINT("GET", "/", root) {
        auto dto = MyDto::createShared();
        dto->statusCode = 200;
        dto->message = "Hello World!";
        gisela->funktionGiessen(30);

        return createDtoResponse(Status::CODE_200, dto);
    }

    ADD_CORS(water)
    ENDPOINT("POST", "/giessen", water, BODY_DTO(Object<WaterDto>, waterDto)) {

        auto dto = MyDto::createShared();
        dto->statusCode = 200;
        dto->message = "giessen gestartet!";
        std::async(&Gisela::funktionGiessen, gisela, waterDto->durationSeconds);
        //gisela->funktionGiessen(waterDto->durationSeconds);

        return createDtoResponse(Status::CODE_200, dto);
    }

    ADD_CORS(settings)
    ENDPOINT("POST", "/save_settings", settings, BODY_DTO(Object<SettingsDto>, settingsDto)) {

        auto dto = MyDto::createShared();
        dto->statusCode = 200;
        dto->message = "settings saved!";
        //Todo settings save

        return createDtoResponse(Status::CODE_200, dto);
    }
};

#include OATPP_CODEGEN_END(ApiController) //<-- End Codegen

#endif //GISELA_RESTAPI_CONTROLLER_H
