#ifndef DTOs_hpp
#define DTOs_hpp

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 *  Data Transfer Object. Object containing fields only.
 *  Used in API for serialization/deserialization and validation
 */
class MyDto : public oatpp::DTO {
  
    DTO_INIT(MyDto, DTO)
  
    DTO_FIELD(Int32, statusCode);
    DTO_FIELD(String, message);
  
};

class WaterDto:public oatpp::DTO {
    DTO_INIT(WaterDto, DTO);
    DTO_FIELD(Int32, durationSeconds);
};

class GetSettingsDto:public oatpp::DTO {
    DTO_INIT(GetSettingsDto, DTO);
    DTO_FIELD(Boolean, b_slot1);
    DTO_FIELD(Boolean, b_slot2);
    DTO_FIELD(Boolean, b_slot3);
    DTO_FIELD(String, t_slot1);
    DTO_FIELD(String, t_slot2);
    DTO_FIELD(String, t_slot3);
    DTO_FIELD(Int32, duration_slot1);
    DTO_FIELD(Int32, duration_slot2);
    DTO_FIELD(Int32, duration_slot3);
    DTO_FIELD(String, lastWater);
    DTO_FIELD(Int32 , level);

};

class LogResponse: public oatpp::DTO {
    DTO_INIT(LogResponse, DTO);
    DTO_FIELD(String, logContent);
};

class SettingsDto:public oatpp::DTO {
    DTO_INIT(SettingsDto, DTO);
    DTO_FIELD(Boolean, b_slot1);
    DTO_FIELD(Boolean, b_slot2);
    DTO_FIELD(Boolean, b_slot3);
    DTO_FIELD(String, t_slot1);
    DTO_FIELD(String, t_slot2);
    DTO_FIELD(String, t_slot3);
    DTO_FIELD(Int32, duration_slot1);
    DTO_FIELD(Int32, duration_slot2);
    DTO_FIELD(Int32, duration_slot3);
    DTO_FIELD(Boolean, waterReset);

    std::string get_slot_string(int slot){
        if (slot == 1){
            std::string slot1 = std::string(b_slot1.getValue(false) ? "1" : "0") + " " +  t_slot1.getValue("") + " ;" + std::to_string(duration_slot1.getValue(0));
            return slot1;
        }
        if (slot == 2){
            std::string slot1 = std::string(b_slot2.getValue(false) ? "1" : "0") + " " +  t_slot2.getValue("") + " ;" + std::to_string(duration_slot2.getValue(0));
            return slot1;
        }
        if (slot == 3){
            std::string slot1 = std::string(b_slot3.getValue(false) ? "1" : "0") + " " +  t_slot3.getValue("") + " ;" + std::to_string(duration_slot3.getValue(0));
            return slot1;
        }

        return "undef";
    }
};

#include OATPP_CODEGEN_END(DTO)

#endif /* DTOs_hpp */