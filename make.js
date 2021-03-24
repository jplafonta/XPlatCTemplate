var path = require("path");

// Making resharper less noisy - These are defined in Generate.js
if (typeof getCompiledTemplate === "undefined") getCompiledTemplate = function () { };
if (typeof templatizeTree === "undefined") templatizeTree = function () { };

exports.makeCombinedAPI = function (apis, sourceDir, apiOutputDir) {
    console.log("Generating Combined api from: " + sourceDir + " to: " + apiOutputDir);

    var removeStatic = ""; // "DISABLE_PLAYFAB_STATIC_API;";
    var libDefines = "ENABLE_PLAYFABADMIN_API;ENABLE_PLAYFABSERVER_API;" + removeStatic;
    var clientDefines = "" + removeStatic;
    var serverDefines = "ENABLE_PLAYFABADMIN_API;ENABLE_PLAYFABSERVER_API;DISABLE_PLAYFABCLIENT_API;" + removeStatic;

    var locals = {
        apis: apis,
        buildIdentifier: sdkGlobals.buildIdentifier,
        clientDefines: clientDefines,
        libDefines: libDefines,
        serverDefines: serverDefines,
        sdkVersion: sdkGlobals.sdkVersion,
        sdkDate: sdkGlobals.sdkVersion.split(".")[2],
        sdkYear: sdkGlobals.sdkVersion.split(".")[2].substr(0, 2),
        vsVer: "v141", // As C++ versions change, we may need to update this
        vsYear: "2017", // As VS versions change, we may need to update this
        getVerticalNameDefault: getVerticalNameDefault,
        winSdkVersion: "10.0.17763.0" // Which version of the Windows SDK (A VS installation option) to use
    };

    templatizeTree(locals, path.resolve(sourceDir, "source"), apiOutputDir);
    for (var a = 0; a < apis.length; a++)
        makeApiFiles(apis[a], sourceDir, apiOutputDir);
};

function makeApiFiles(api, sourceDir, apiOutputDir) {
    var remStaticDefine = ""; // " && !defined(DISABLE_PLAYFAB_STATIC_API)";

    var locals = {
        api: api,
        prefix: getPrefix(api.name),
        enumtypes: getEnumTypes(api.datatypes),
        getApiDefine: getApiDefine,
        getAuthParams: getAuthParams,
        getBaseType: getBaseType,
        getPrefix: getPrefix,
        getPropertyDefinition: getPropertyDefinition,
        getPropertyDefinitionC: getPropertyDefinitionC,
        getPropertyFromJson: getPropertyFromJson,
        getPropertyToJson: getPropertyToJson,
        getRequestActions: getRequestActions,
        getResultActions: getResultActions,
        canDefaultCopyConstructor: canDefaultCopyConstructor,
        getCopyConstructorInitializationList: getCopyConstructorInitializationList,
        getCopyConstructorBody: getCopyConstructorBody,
        hasClientOptions: getAuthMechanisms([api]).includes("SessionTicket"),
        hasAuthParams: hasAuthParams,
        ifHasProps: ifHasProps,
        remStaticDefine: remStaticDefine,
        sdkVersion: sdkGlobals.sdkVersion,
        sortedClasses: getSortedClasses(api.datatypes)
    };

    var iapihTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/PlayFab_InstanceApi.h.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/include/playfab", "PlayFab" + api.name + "InstanceApi.h"), iapihTemplate(locals));

    var iapiCppTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/PlayFab_InstanceApi.cpp.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/source/playfab", "PlayFab" + api.name + "InstanceApi.cpp"), iapiCppTemplate(locals));

    var dataModelTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/PlayFab_DataModels.h.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/include/playfab", "PlayFab" + api.name + "DataModels.h"), dataModelTemplate(locals));

    var dataModelTemplate_c = getCompiledTemplate(path.resolve(sourceDir, "templates/PlayFab_DataModels_c.h.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/include/playfab", "PlayFab" + api.name + "DataModels_c.h"), dataModelTemplate_c(locals));
}

// *************************** Internal utility methods ***************************
function getEnumTypes(datatypes) {
    var enumtypes = [];

    for (var typeIdx in datatypes) // Add all types and their dependencies
        if (datatypes[typeIdx].isenum)
            enumtypes.push(datatypes[typeIdx]);
    return enumtypes;
}

function getSortedClasses(datatypes) {
    var sortedClasses = [];
    var addedTypes = new Set();

    var addType = function (datatype) {
        if (addedTypes.has(datatype.name) || datatype.isenum)
            return;
        // In C++, dependent types must be defined first
        if (datatype.properties) {
            for (var propIdx = 0; propIdx < datatype.properties.length; propIdx++) {
                var property = datatype.properties[propIdx];
                if (property.isclass || property.isenum)
                    addType(datatypes[property.actualtype]);
            }
        }
        addedTypes.add(datatype.name);
        sortedClasses.push(datatype);
    };

    for (var typeIdx in datatypes) // Add all types and their dependencies
        addType(datatypes[typeIdx]);
    return sortedClasses;
}

// *************************** ejs-exposed methods ***************************
function getApiDefine(api) {
    if (api.name === "Client")
        return "#if !defined(DISABLE_PLAYFABCLIENT_API)";
    if (api.name === "Matchmaker")
        return "#if defined(ENABLE_PLAYFABSERVER_API)"; // Matchmaker is bound to server, which is just a legacy design decision at this point
    if (api.name === "Admin" || api.name === "Server")
        return "#if defined(ENABLE_PLAYFAB" + api.name.toUpperCase() + "_API)";

    // For now, everything else is considered ENTITY
    return "#if !defined(DISABLE_PLAYFABENTITY_API)";
}

function hasAuthParams(apiCall) {
    try {
        getAuthParams(apiCall, true);
    } catch (err) {
        return false;
    }
    return true;
}

function getAuthParams(apiCall) {
    if (apiCall.url === "/Authentication/GetEntityToken")
        return "authKey, authValue";
    switch (apiCall.auth) {
        case "EntityToken": return "\"X-EntityToken\", m_context->entityToken.data()";
        case "SessionTicket": return "\"X-Authorization\", m_context->clientSessionTicket.data()";
        case "SecretKey": return "\"X-SecretKey\", m_settings->developerSecretKey.data()";
    }
    
    throw Error("getAuthParams: Unknown auth type: " + apiCall.auth + " for " + apiCall.name);
}
function getBaseType(datatype) {
    if (datatype.className.toLowerCase().endsWith("request"))
        return "PlayFabRequestCommon";
    if (datatype.className.toLowerCase().endsWith("loginresult"))
        return "PlayFabLoginResultCommon";
    if (datatype.className.toLowerCase().endsWith("response") || datatype.className.toLowerCase().endsWith("result"))
        return "PlayFabResultCommon";
    return "PlayFabBaseModel";
}

function getPropertyCppType(property, datatype, needOptional, apiName) {
    var isOptional = property.optional && needOptional;

    if (property.actualtype === "String")
        return "String"; // Do we want to distinguish optional values here?
    else if (property.isclass)
        return isOptional ? ("StdExtra::optional<" + property.actualtype + ">") : property.actualtype;
    else if (property.jsontype === "Object" && property.actualtype === "object")
        return "JsonValue";
    else if (property.actualtype === "Boolean")
        return isOptional ? "StdExtra::optional<bool>" : "bool";
    else if (property.actualtype === "int16")
        return isOptional ? "StdExtra::optional<Int16>" : "Int16";
    else if (property.actualtype === "uint16")
        return isOptional ? "StdExtra::optional<Uint16>" : "Uint16";
    else if (property.actualtype === "int32")
        return isOptional ? "StdExtra::optional<Int32>" : "Int32";
    else if (property.actualtype === "uint32")
        return isOptional ? "StdExtra::optional<Uint32>" : "Uint32";
    else if (property.actualtype === "int64")
        return isOptional ? "StdExtra::optional<Int64>" : "Int64";
    else if (property.actualtype === "uint64")
        return isOptional ? "StdExtra::optional<Uint64>" : "Uint64";
    else if (property.actualtype === "float")
        return isOptional ? "StdExtra::optional<float>" : "float";
    else if (property.actualtype === "double")
        return isOptional ? "StdExtra::optional<double>" : "double";
    else if (property.actualtype === "DateTime")
        return isOptional ? "StdExtra::optional<time_t>" : "time_t";
    else if (property.isenum)
        return isOptional ? ("StdExtra::optional<" + getPrefix(apiName) + property.actualtype + ">") : (getPrefix(apiName) + property.actualtype);
    throw Error("getPropertyCppType: Unknown property type: " + property.actualtype + " for " + property.name + " in " + datatype.name);
}

function getPropertyDefinition(tabbing, property, datatype, apiName) {
    var cppType = getPropertyCppType(property, datatype, !property.collection, apiName);

    if (!property.collection) {
        return tabbing + cppType + " " + property.name + ";";
    } else if (property.jsontype === "Object" && property.actualtype === "object") {
        return tabbing + cppType + " " + property.name + ";";
    } else if (property.collection === "array") {
        return tabbing + "List<" + cppType + "> " + property.name + ";";
    } else if (property.collection === "map") {
        return tabbing + "Map<String, " + cppType + "> " + property.name + ";";
    }
    throw Error("getPropertyDefinition: Unknown property type: " + property.actualtype + " for " + property.name + " in " + datatype.name);
}

function getPrefix(apiName) {
    return "PlayFab" + apiName;
}

function getDictionaryEntryTypeName(property) {
    return "PlayFab" + property.actualtype + "DictionaryEntry";
}

function getPropertyCType(property, apiName) {

    if (property.collection === "map") {
        return "struct " + getDictionaryEntryTypeName(property) + "*";
    } else if (property.actualtype === "String") {
        if (!property.collection) {
            return "const char*";
        } else {
            return "const char**";
        }
    } else if (property.jsontype === "Object" && property.actualtype === "object") {
        return "PlayFabJsonString";
    } else if (property.isclass) {
        // By design class properties are always pointers. This allows us to use a derived class internally which can help with
        // lifetime management, copying models, etc. by implementing copy constructors & destructors
        if (!property.collection) {
            return getPrefix(apiName) + property.actualtype + "*";
        } else {
            // array of pointers
            return getPrefix(apiName) + property.actualtype + "**";
        }
    } else if (property.isenum) {
        if (!property.collection && !property.optional) {
            return getPrefix(apiName) + property.actualtype;
        } else {
            // type for optional value & array value happens to be the same
            return getPrefix(apiName) + property.actualtype + "*";
        }
    } else {
        var type;
        switch (property.actualtype) {
            case "Boolean": {
                type = "bool";
                break;
            }
            case "int16": {
                type = "int16_t";
                break;
            }
            case "uint16": {
                type = "uint16_t";
                break;
            }
            case "int32": {
                type = "int32_t";
                break;
            }
            case "uint32": {
                type = "uint32_t";
                break;
            }
            case "int64": {
                type = "int64_t";
                break;
            }
            case "uint64": {
                type = "uint64_t";
                break;
            }
            case "float": {
                type = "float";
                break;
            }
            case "double": {
                type = "double";
                break;
            }
            case "DateTime": {
                type = "time_t";
                break;
            }
            default: {
                throw Error("Unexpected type: " + property.actualtype);
            }
        }
        if (property.collection === "array" || property.optional) {
            type += "*";
        }
        return type;
    }
}

function getPropertyName(property) {
    var name = property.name;

    // Don't allow property name to be a C++ reserved word
    if (name.localeCompare("namespace", undefined, { sensitivity: 'accent' }) === 0) {
        name = "playfab" + name;
    }

    // GameCore design guideline is to use camelcase for property names (service is using Pascal case)
    name = name.replace(/(?:^\w|[A-Z]|\b\w|\s+)/g, function (match, index) {
        if (+match === 0) return ""; // or if (/\s+/.test(match)) for white spaces
        return index === 0 ? match.toLowerCase() : match.toUpperCase();
    });

    return name;
}

function getPropertyDefinitionC(tabbing, property, apiName) {
    var type = getPropertyCType(property, apiName);
    var propName = getPropertyName(property);
    var output = tabbing + type + " " + propName + ";";

    // For collection properties add an addition "count" property
    if (property.collection) {
        output += ("\n" + tabbing + "uint32_t " + propName + "Count;"); 
    }

    return output;
}

function getPropertyFromJson(tabbing, property, datatype) {
    if (property.actualtype === "DateTime") {
        // Special case needed for time since we are translating from JsonString -> c++ time_t
        return tabbing + "JsonUtils::ObjectGetMemberTime(input, \"" + property.name + "\", " + property.name + ");";
    }

    return tabbing + "JsonUtils::ObjectGetMember(input, \"" + property.name + "\", " + property.name + ");";
}

function getPropertyToJson(tabbing, property, datatype) {
    if (property.actualtype === "DateTime") {
        // Special case needed for time since we are translating from c++ time_t -> JsonString
        return tabbing + "JsonUtils::ObjectAddMemberTime(output, \"" + property.name + "\", " + property.name + ");";
    }
    return tabbing + "JsonUtils::ObjectAddMember(output, \"" + property.name + "\", " + property.name + ");";
}

function canDefaultCopyConstructor(datatype) {
    if (datatype.properties) {
        for (var propIdx = 0; propIdx < datatype.properties.length; propIdx++) {
            var property = datatype.properties[propIdx];
            if (property.jsontype === "Object" && property.actualtype === "object") {
                return false;
            }
        }
    }
    return true;
}

function getCopyConstructorInitializationList(tabbing, datatype) {
    var initializationList = "";
    var firstProp = true;
    for (var propIdx = 0; propIdx < datatype.properties.length; propIdx++) {
        var property = datatype.properties[propIdx];
        if (!(property.jsontype === "Object" && property.actualtype === "object")) {
            if (firstProp) {
                firstProp = false;
                initializationList += (tabbing + ": " + property.name + "{ src." + property.name + " }");
            } else {
                initializationList += (",\n" + tabbing + property.name + "{ src." + property.name + " }");
            }
        }
    }
    return initializationList;
}

function getCopyConstructorBody(tabbing, datatype) {
    var constructorBody = "";
    var firstProp = true;
    for (var propIdx = 0; propIdx < datatype.properties.length; propIdx++) {
        var property = datatype.properties[propIdx];
        if (property.jsontype === "Object" && property.actualtype === "object") {
            if (firstProp) {
                firstProp = false;
                constructorBody += (tabbing + "JsonUtils::FromJson(src." + property.name + ", " + property.name + ");");
            } else {
                constructorBody += ("\n" + tabbing + "JsonUtils::FromJson(src." + property.name + ", " + property.name + ");");
            }
        }
    }
    return constructorBody;
}

function getRequestActions(tabbing, apiCall) {
    if (apiCall.result === "LoginResult" || apiCall.result === "RegisterPlayFabUserResult")
        return tabbing + "m_settings->titleId = request.TitleId;\n"

    if (apiCall.url === "/Authentication/GetEntityToken")
        return tabbing + "String authKey, authValue;\n" +
            tabbing + "if (m_context->entityToken.length() > 0)\n" +
            tabbing + "{\n" +
            tabbing + "    authKey = \"X-EntityToken\"; authValue = m_context->entityToken.data();\n" +
            tabbing + "}\n" +
            tabbing + "else if (m_context->clientSessionTicket.length() > 0)\n" +
            tabbing + "{\n" +
            tabbing + "    authKey = \"X-Authorization\"; authValue = m_context->clientSessionTicket.data();\n" +
            tabbing + "}\n" +
            "#if defined(ENABLE_PLAYFABSERVER_API) || defined(ENABLE_PLAYFABADMIN_API)\n" +
            tabbing + "else if (m_settings->developerSecretKey.length() > 0)\n" +
            tabbing + "{\n" +
            tabbing + "    authKey = \"X-SecretKey\"; authValue = m_settings->developerSecretKey.data();\n" +
            tabbing + "}\n" +
            "#endif\n";

    return "";
}

function getResultActions(tabbing, apiCall) {
    if (apiCall.url === "/Authentication/GetEntityToken")
        return tabbing + "context->HandlePlayFabLogin(\"\", \"\", outResult.Entity->Id, outResult.Entity->Type, outResult.EntityToken);\n";
    if (apiCall.result === "LoginResult")
        return tabbing + "outResult.authenticationContext = std::make_shared<PlayFabAuthenticationContext>();\n" +
            tabbing + "outResult.authenticationContext->HandlePlayFabLogin(outResult.PlayFabId, outResult.SessionTicket, outResult.EntityToken->Entity->Id, outResult.EntityToken->Entity->Type, outResult.EntityToken->EntityToken);\n" +
            tabbing + "context->HandlePlayFabLogin(outResult.PlayFabId, outResult.SessionTicket, outResult.EntityToken->Entity->Id, outResult.EntityToken->Entity->Type, outResult.EntityToken->EntityToken);\n" +
            tabbing + "MultiStepClientLogin(context, outResult.SettingsForUser->NeedsAttribution);\n";
    if (apiCall.result === "RegisterPlayFabUserResult")
        return tabbing + "context->HandlePlayFabLogin(outResult.PlayFabId, outResult.SessionTicket, outResult.EntityToken->Entity->Id, outResult.EntityToken->Entity->Type, outResult.EntityToken->EntityToken);\n"
            + tabbing + "MultiStepClientLogin(context, outResult.SettingsForUser->NeedsAttribution);\n";
    if (apiCall.result === "AttributeInstallResult")
        return tabbing + "context->advertisingIdType += \"_Successful\";\n";

    return "";
}

function ifHasProps(datatype, displayText) {
    if (datatype.properties.length === 0)
        return "";
    return displayText;
}

function getVerticalNameDefault() {
    if (sdkGlobals.verticalName) {
        return sdkGlobals.verticalName;
    }
    return "";
}
