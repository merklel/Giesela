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
    std::shared_ptr<ObjectMapper> objectMapper;
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
        this->objectMapper = objectMapper;
    }
public:

    ENDPOINT("GET", "/", root) {
        auto dto = MyDto::createShared();
        dto->statusCode = 200;
        dto->message = "Hello World!";
        return createDtoResponse(Status::CODE_200, dto);
    }

    ADD_CORS(water, "*", "POST, GET", "DNT, User-Agent, X-Requested-With, If-Modified-Since, Cache-Control, Content-Type, Range")
    ENDPOINT("POST", "/giessen", water, BODY_DTO(Object<WaterDto>, waterDto), REQUEST(std::shared_ptr<IncomingRequest>, request)) {

        auto dto = MyDto::createShared();
        dto->statusCode = 200;
        dto->message = "giessen gestartet!";

        if(waterDto->durationSeconds) {
            OATPP_LOGD("giessen", "duration='%d'", waterDto->durationSeconds.getValue(15))
        }else{
            OATPP_LOGD("giessen", "No valid giessen body found")
        }

        std::async(std::launch::async, &Gisela::funktionGiessen, gisela, waterDto->durationSeconds.getValue(15));
        //gisela->funktionGiessen(waterDto->durationSeconds);

        return createDtoResponse(Status::CODE_200, dto);
    }

    ADD_CORS(settings, "*", "POST, GET", "DNT, User-Agent, X-Requested-With, If-Modified-Since, Cache-Control, Content-Type, Range")
    ENDPOINT("POST", "/saveSettings", settings, BODY_DTO(Object<SettingsDto>, settingsDto)) {

        OATPP_LOGD("saveSettings", "slot1: %s", settingsDto->get_slot_string(1).c_str());
        OATPP_LOGD("saveSettings", "slot2: %s", settingsDto->get_slot_string(2).c_str());
        OATPP_LOGD("saveSettings", "slot3: %s", settingsDto->get_slot_string(3).c_str());

        auto dto = MyDto::createShared();
        dto->statusCode = 200;
        dto->message = "settings saved!";
        gisela->set_current_config_via_api(settingsDto->b_slot1.getValue(false),
                                           settingsDto->b_slot2.getValue(false),
                                           settingsDto->b_slot3.getValue(false),
                                           settingsDto->t_slot1.getValue("10:00"),
                                           settingsDto->t_slot2.getValue("10:00"),
                                           settingsDto->t_slot3.getValue("10:00"),
                                           settingsDto->duration_slot1.getValue(5),
                                           settingsDto->duration_slot2.getValue(5),
                                           settingsDto->duration_slot3.getValue(5));
        gisela->writeJsonConfigToDisk();
        return createDtoResponse(Status::CODE_200, dto);
    }

    ADD_CORS(getSettings, "*", "POST, GET", "DNT, User-Agent, X-Requested-With, If-Modified-Since, Cache-Control, Content-Type, Range")
    ENDPOINT("GET", "/getSettings", getSettings) {
        auto dto = GetSettingsDto::createShared();
        json conf = gisela->getJsonConfig();
        dto->t_slot1 = conf["t_slot1"].template get<std::string>();
        dto->t_slot2 = conf["t_slot2"].template get<std::string>();
        dto->t_slot3 = conf["t_slot3"].template get<std::string>();
        dto->b_slot1 = conf["b_slot1"].template get<bool>();
        dto->b_slot2 = conf["b_slot2"].template get<bool>();
        dto->b_slot3 = conf["b_slot3"].template get<bool>();
        dto->duration_slot1 = conf["duration_slot1"].template get<int>();
        dto->duration_slot2 = conf["duration_slot2"].template get<int>();
        dto->duration_slot3 = conf["duration_slot3"].template get<int>();
        dto->lastWater = conf["lastWater"].template get<std::string>();
        return createDtoResponse(Status::CODE_200, dto);
    }

    ADD_CORS(getLog, "*", "POST, GET", "DNT, User-Agent, X-Requested-With, If-Modified-Since, Cache-Control, Content-Type, Range")
    ENDPOINT("GET", "/getLog", getLog) {
        auto logResponse = LogResponse::createShared();
        logResponse->logContent =  gisela->getLog();
        return createDtoResponse(Status::CODE_200, logResponse);
    }
};

#include OATPP_CODEGEN_END(ApiController) //<-- End Codegen

#endif //GISELA_RESTAPI_CONTROLLER_H
