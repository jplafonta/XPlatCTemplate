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
        hasAuthCall: hasAuthCall,
        projectFiles: parseProjectFiles("project_files.json"),
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
    var prefix = "PlayFab" + api.name;

    pruneEmptyTypes(api);
    var authCalls = getAuthCalls(api);

    var locals = {
        api: api,
        authCalls: authCalls,
        prefix: prefix,
        enumtypes: getEnumTypes(api.datatypes),
        getApiDefine: getApiDefine,
        getAuthParams: getAuthParams,
        getBaseType: getBaseType,
        getPropertyDefinition: getPropertyDefinition,
        getPropertyFromJson: getPropertyFromJson,
        addPropertyToJson: addPropertyToJson,
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
        sortedClasses: getSortedClasses(api.datatypes),
        dictionaryEntryTypes: getDictionaryEntryTypes(api.datatypes)
    };

    var iapihTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/_Api.h.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/source/" + api.name, api.name + "Api.h"), iapihTemplate(locals));

    var iapiCppTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/_Api.cpp.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/source/" + api.name, api.name + "Api.cpp"), iapiCppTemplate(locals));

    var dataModelTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/_DataModels.h.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/source/" + api.name, api.name + "DataModels.h"), dataModelTemplate(locals));

    var dataModelTemplate_c = getCompiledTemplate(path.resolve(sourceDir, "templates/PlayFab_DataModels_c.h.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/include/playFab", "PlayFab" + api.name + "DataModels_c.h"), dataModelTemplate_c(locals));

    if (authCalls.length > 0) {
        var authApiHeaderTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/_AuthApi.h.ejs"));
        writeFile(path.resolve(apiOutputDir, "code/source/" + api.name, api.name + "AuthApi.h"), authApiHeaderTemplate(locals));

        var authApiTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/_AuthApi.cpp.ejs"));
        writeFile(path.resolve(apiOutputDir, "code/source/" + api.name, api.name + "AuthApi.cpp"), authApiTemplate(locals));

        var publicAuthApiHeaderTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/PlayFab_AuthApi.h.ejs"));
        writeFile(path.resolve(apiOutputDir, "code/include/playfab", "PlayFab" + api.name + "AuthApi.h"), publicAuthApiHeaderTemplate(locals));

        var publicAuthApiTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/PlayFab_AuthApi.cpp.ejs"));
        writeFile(path.resolve(apiOutputDir, "code/source/" + api.name, "PlayFab" + api.name + "AuthApi.cpp"), publicAuthApiTemplate(locals));
    }
}

// *************************** Internal utility methods ***************************
function parseProjectFiles(filename) {
    var fullPath = path.resolve(__dirname, filename);
    console.log("Begin reading File: " + fullPath);
    var projectFiles = null;
    try {
        projectFiles = require(fullPath);
    }
    catch (err) {
        console.log(" ***** Failed to Load: " + fullPath);
        throw err;
    }
    console.log("Finished reading: " + fullPath);
    return projectFiles;
}

function pruneEmptyTypes(api) {

    var datatypes = api.datatypes;

    // Prune properties whose type is an empty class
    for (var typeIdx in datatypes) {
        var datatype = datatypes[typeIdx];
        if (datatype.properties) {
            for (var propIdx = datatype.properties.length - 1; propIdx >= 0; propIdx--) {
                var property = datatype.properties[propIdx];
                if (property.isclass) {
                    var propertyDataType = datatypes[property.actualtype]
                    if (!propertyDataType.properties || propertyDataType.properties.length === 0) {
                        datatype.properties.splice(propIdx, 1);
                    }
                }
            }
        }
    }

    // Prune datatypes which are empty classes
    for (typeIdx in datatypes) {
        datatype = datatypes[typeIdx];
        if (datatype.properties && datatype.properties.length === 0) {
            delete datatypes[typeIdx];
        }
    }

    // Change request and response types if they are empty classes
    var calls = api.calls;
    for (var callIdx = 0; callIdx < api.calls.length; callIdx++) {
        var apiCall = calls[callIdx];
        if (!(apiCall.request in datatypes)) {
            apiCall.request = "BaseRequest";
        }
        if (!(apiCall.result in datatypes)) {
            apiCall.result = "BaseResult";
        }
    }

}

function getAuthCalls(api) {
    // TODO probably better to remove these from api.calls rather than copy, but just copying for now

    var authCalls = [];

    for (var callIdx = 0; callIdx < api.calls.length; callIdx++) {
        var call = api.calls[callIdx];
        if (isAuthCall(call)) {
            authCalls.push(call) 
        }
    }
    return authCalls;
}

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
                if (property.isclass || property.isenum) {
                    addType(datatypes[property.actualtype]);
                }
            }
        }
        addedTypes.add(datatype.name);
        sortedClasses.push(datatype);
    };

    // Add all types and their dependencies
    for (var typeIdx in datatypes) {
        addType(datatypes[typeIdx]);
    }
    return sortedClasses;
}

function getDictionaryEntryTypes(datatypes) {
    // Gets all (non-primitive) types which appear as a Value in a map
    var dictionaryEntryTypes = [];
    var addedTypes = new Set();

    for (var typeIdx in datatypes) {
        var datatype = datatypes[typeIdx];
        if (datatype.properties) {
            for (var propIdx = 0; propIdx < datatype.properties.length; propIdx++) {
                var property = datatype.properties[propIdx];
                if (property.collection === "map" && (property.isclass || property.isenum) && !addedTypes.has(property.actualtype)) {
                    dictionaryEntryTypes.push(datatypes[property.actualtype]);
                    addedTypes.add(property.actualtype);
                }
            }
        }
    }
    return dictionaryEntryTypes;
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

// Check if an API has any calls that acquire auth tokens
function hasAuthCall(api) {
    for (var callIdx = 0; callIdx < api.calls.length; callIdx++) {
        if (isAuthCall(api.calls[callIdx])) {
            return true;
        }
    }
    return false;
}

// return if an API call is authentication call
function isAuthCall(call) {
    // TODO this might not be the only check needed
    // Could we check for call.auth === "none"?
    if (call.result.toLowerCase().endsWith("loginresult")) {
        return true;
    } else if (call.result === "RegisterPlayFabUserResult") {
        return true;
    }
    return false;
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
        return "BaseRequest";
    if (datatype.className.toLowerCase().endsWith("loginresult"))
        return "PlayFabLoginResultCommon";
    if (datatype.className.toLowerCase().endsWith("response") || datatype.className.toLowerCase().endsWith("result"))
        return "BaseResult";
    return "BaseModel";
}

function getDictionaryEntryTypeFromValueType(valueType) {
    var types = {
        "String": "String", "const char*": "String", "bool": "Bool", "int16_t": "Int16", "uint16_t": "Uint16", "int32_t": "Int32", "uint32_t": "Uint32",
        "int64_t": "int64_t", "uint64_t": "int64_t", "float": "float", "double": "double", "time_t": "DateTime"
    };

    if (valueType in types) {
        return "PlayFab" + types[valueType] + "DictionaryEntry";
    } else {
        // valueType should already be prefixed appropriately
        return valueType + "DictionaryEntry";
    }
}

function requiresInternalProperty(property) {
    // An internal property is needed if the public property requires dynamic storage.
    // Dynamic storage is not required for non-optional/non-collection primitive & enum types
    var primitives = new Set(["Boolean", "int16", "uint16", "int32", "uint32", "int64", "uint64", "float", "double", "DateTime"]);

    if ((primitives.has(property.actualtype) || property.isenum) && !property.optional && !property.collection) {
        return false;
    }
    return true;
}

function getInternalPropertyType(property, prefix) {
    var type = "";

    // Service types that can be mapped directly to C++ types
    var types = {
        "String": "String", "Boolean": "bool", "int16": "int16_t", "uint16": "uint16_t", "int32": "int32_t", "uint32": "uint32_t",
        "int64": "int64_t", "uint64": "uint64_t", "float": "float", "double": "double", "DateTime": "time_t", "object": "JsonObject"
    };

    if (property.actualtype in types) {
        type = types[property.actualtype];
    } else if (property.isclass) {
        type = property.actualtype;
    } else if (property.isenum) {
        // Enums always defined in global namespace so add prefix
        type = prefix + property.actualtype
    } else {
        throw Error("Unrecognized property type " + property.actualtype);
    }

    // Modify type depending on collection & optional attributes. 
    // TODO describe reasons for this breakdown somewhere (here or in source)
    if (!(property.actualtype === "object")) {
        if (property.collection === "map") {
            if (property.isclass) {
                return "AssociativeArray<" + getDictionaryEntryTypeFromValueType(prefix + type) + ", " + type + ">";
            } else if (type === "String") {
                return "AssociativeArray<PlayFabStringDictionaryEntry, String>";
            } else {
                return "AssociativeArray<" + getDictionaryEntryTypeFromValueType(type) + ", void>";
            }
        } else if (property.collection === "array") {
            if (property.isclass) {
                return "PointerArray<" + prefix + type + ", " + type + ">";
            } else if (type === "String") {
                return "PointerArray<const char, String>";
            } else {
                return "Vector<" + type + ">";
            }
        } else if (property.optional) {
            // Types which aren't nullable will be wrapped by StdExtra::optional.
            // String is an exception here - empty will represent not set(might want to revisit this design)
            if (!(type === "String" || type === "JsonObject")) {
                return "StdExtra::optional<" + type + ">";
            }
        }
    }

    return type;
}

function getPublicPropertyType(property, prefix) {
    var type = "";

    // Service types that can be mapped directly to C types
    var types = {
        "String": "const char*", "Boolean": "bool", "int16": "int16_t", "uint16": "uint16_t", "int32": "int32_t", "uint32": "uint32_t",
        "int64": "int64_t", "uint64": "uint64_t", "float": "float", "double": "double", "DateTime": "time_t", "object": "PlayFabJsonObject"
    };

    if (property.actualtype in types) {
        type = types[property.actualtype];
    } else if (property.isclass || property.isenum) {
        // TODO should we include 'struct' keyword to distinguish structs vs enums
        type = prefix + property.actualtype;
    } else {
        throw Error("Unrecognized property type " + property.actualtype);
    }

    // By design class properties are always pointers. Pointers will ultimately point to derived C++ internal Objects which
    // can automatically manage their cleanup & copying via destructors and copy constructors

    // Modify type depending on collection & optional attributes
    // TODO figure out const correctness here
    if (!(property.actualtype === "object")) {
        if (property.collection === "map") {
            // array of dictionary entries
            return "PF_MAP struct " + getDictionaryEntryTypeFromValueType(type) + "*";
        } else if (property.collection === "array") {
            if (property.isclass) {
                // array of pointers
                return "PF_ARRAY " + type + "**";
            } else {
                return "PF_ARRAY " + type + "*";
            }
        } else if (property.optional) {
            // Types which aren't already nullable will be made pointer types
            if (!(type === "const char*" || type === "PlayFabJsonObject")) {
                return "PF_OPTIONAL " + type + "*";
            }
        }
    }

    if (property.isclass) {
        return type + "*";
    }

    return type;
}

function getPropertyName(property, isInternal) {
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

    return isInternal ? "m_" + name : name;
}

function getPropertyDefinition(tabbing, property, prefix, isInternal) {
    var output = "";

    if (!isInternal || requiresInternalProperty(property)) {
        var type = isInternal ? getInternalPropertyType(property, prefix) : getPublicPropertyType(property, prefix);
        var propName = getPropertyName(property, isInternal);
        output = "\n" + tabbing + type + " " + propName + ";";

        // For public collection properties add an additional "count" property
        if (property.collection && !(type === "PlayFabJsonObject") && !isInternal) {
            output += ("\n" + tabbing + "PF_COLLECTION_COUNT uint32_t " + propName + "Count;");
        }
    }
    return output;
}

function getPropertyFromJson(tabbing, property) {
    var publicPropName = getPropertyName(property, false);
    var internalPropName = getPropertyName(property, true);

    var output = tabbing + "JsonUtils:: ObjectGetMember(input, \"" + property.name + "\", ";

    if (!requiresInternalProperty(property)) {
        output += publicPropName;
    } else {
        // if there is internal storage for a property, get the internal value and the public pointer to it
        output += (internalPropName + ", " + publicPropName);
    }

    if (property.collection && !(property.actualtype === "object")) {
        // for collections, also retreive the collection count
        output += (", " + publicPropName + "Count");
    }

    // for DateTime values, set optional param "convertToIso8601String" to true
    if (property.actualtype === "DateTime") {
        output += ", true";
    }

    return output + ");";
}

function addPropertyToJson(tabbing, property) {
    var publicPropName = getPropertyName(property, false);

    var output = tabbing + "JsonUtils::ObjectAddMember(output, \"" + property.name + "\", input." + publicPropName;

    // for collections, pass the collection count
    if (property.collection && !(property.actualtype === "object")) {
        output += ", input." + publicPropName + "Count";
    }

    // for DateTime values, set optional param "convertToIso8601String" to true
    if (property.actualtype === "DateTime") {
        output += ", true"; 
    }

    return output + ");";
}

function canDefaultCopyConstructor(datatype) {
    // We can default the copy construct iff there are no internal properties (i.e. no public properties reference memory outside the struct)
    if (datatype.properties) {
        for (var propIdx = 0; propIdx < datatype.properties.length; propIdx++) {
            var property = datatype.properties[propIdx];
            if (requiresInternalProperty(property)) {
                return false;
            }
        }
    }
    return true;
}

function getCopyConstructorInitializationList(tabbing, datatype) {
    var output = "";
    for (var propIdx = 0; propIdx < datatype.properties.length; propIdx++) {
        var property = datatype.properties[propIdx];
        if (requiresInternalProperty(property)) {
            var internalPropName = getPropertyName(property, true);
            output += (",\n" + tabbing + internalPropName + "{ src." + internalPropName + " }");
        }
    }
    return output;
}

function getCopyConstructorBody(tabbing, datatype, prefix) {
    var output = "";
    for (var propIdx = 0; propIdx < datatype.properties.length; propIdx++) {
        var property = datatype.properties[propIdx];
        if (requiresInternalProperty(property)) {
            var publicPropName = getPropertyName(property, false);
            var internalPropName = getPropertyName(property, true);
            var internalPropertyType = getInternalPropertyType(property, prefix);

            if (internalPropertyType === "JsonObject") {
                output += ("\n" + tabbing + publicPropName + ".stringValue = " + internalPropName + ".StringValue();");
            } else if (internalPropertyType.includes("Array")) {
                output += ("\n" + tabbing + publicPropName + " = " + internalPropName + ".Empty() ? nullptr : " + internalPropName + ".Data();");
            } else if (internalPropertyType === "String" || internalPropertyType.includes("Vector")) {
                output += ("\n" + tabbing + publicPropName + " = " + internalPropName + ".empty() ? nullptr : " + internalPropName + ".data();");
            } else if (property.optional) {
                output += ("\n" + tabbing + publicPropName + " = " + internalPropName + " ? " + internalPropName + ".operator->() : nullptr;");
            } else if (property.isclass) {
                output += ("\n" + tabbing + publicPropName + " = (" + getPublicPropertyType(property, prefix) + ")&" + internalPropName + ";");
            } else {
                throw Error("Unable to copy property of type " + property.actualtype);
            }
        }
    }
    return output;
}

function getRequestActions(tabbing, apiCall) {
    if (apiCall.result === "LoginResult" || apiCall.result === "RegisterPlayFabUserResult")
        return tabbing + "m_settings->titleId = request.titleId;\n"

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
        return tabbing + "context->HandlePlayFabLogin(\"\", \"\", outResult.entity->id, outResult.entity->type, outResult.entityToken);\n";
    if (apiCall.result === "LoginResult")
        return tabbing + "outResult.authenticationContext = std::make_shared<PlayFabAuthenticationContext>();\n" +
            tabbing + "outResult.authenticationContext->HandlePlayFabLogin(outResult.playFabId, outResult.sessionTicket, outResult.entityToken->entity->id, outResult.entityToken->entity->type, outResult.entityToken->entityToken);\n" +
            tabbing + "context->HandlePlayFabLogin(outResult.playFabId, outResult.sessionTicket, outResult.entityToken->entity->id, outResult.entityToken->entity->type, outResult.entityToken->entityToken);\n" +
            tabbing + "MultiStepClientLogin(context, outResult.settingsForUser->needsAttribution);\n";
    if (apiCall.result === "RegisterPlayFabUserResult")
        return tabbing + "context->HandlePlayFabLogin(outResult.playFabId, outResult.sessionTicket, outResult.entityToken->entity->id, outResult.entityToken->entity->type, outResult.entityToken->entityToken);\n"
            + tabbing + "MultiStepClientLogin(context, outResult.settingsForUser->needsAttribution);\n";
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
