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

class SettingsDto:public oatpp::DTO {
    DTO_INIT(SettingsDto, DTO);
    DTO_FIELD(Boolean, b_slot1);
    DTO_FIELD(Boolean, b_slot2);
    DTO_FIELD(Boolean, b_slot3);
    DTO_FIELD(String, t_slot1);
    DTO_FIELD(String, t_slot2);
    DTO_FIELD(String, t_slot3);
    DTO_FIELD(Float64, level);
    DTO_FIELD(Boolean, waterReset);
    DTO_FIELD(String , lastWater);
};

#include OATPP_CODEGEN_END(DTO)

#endif /* DTOs_hpp */