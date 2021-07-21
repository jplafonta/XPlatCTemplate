var path = require("path");

// Making resharper less noisy - These are defined in Generate.js
if (typeof getCompiledTemplate === "undefined") getCompiledTemplate = function () { };
if (typeof templatizeTree === "undefined") templatizeTree = function () { };

var categorizedApis = {};
var xmlRefDocs = {};
var propertyReplacements = {};
var globalPrefix = "PlayFab"; // Global prefix for all public types
var testStatusMap = {};
var apiGrouping = {};

// Shared API. Structured the same as other api objects so it can be used the same in template files. Used to generate shared DataModels
var sharedApi = {
    "name": "Shared",
    "datatypes": {},
    "calls": [],
    "dictionaryEntryTypes": {}
}; 

exports.makeCombinedAPI = function (apis, sourceDir, apiOutputDir) {
    console.log("Generating Combined api from: " + sourceDir + " to: " + apiOutputDir);

    try {
        propertyReplacements = require(path.resolve(sourceDir, "replacements.json"));
    } catch (ex) {
        throw "The file: replacements.json was not properly formatted JSON";
    }

    // Populate SharedApi and add to apis
    populateSharedDatatypes(apis);
    apis.push(sharedApi);

    categorizeCalls(apis);

    xmlRefDocs = parseDataFile("XMLRefDocs.json");
    testStatusMap = parseDataFile("TestStatus.json");
    apiGrouping = parseDataFile("APIGrouping.json");

    var locals = {
        apis: apis,
        categorizedApis: categorizedApis,
        projectFiles: parseProjectFiles("project_files.json"),
        buildIdentifier: sdkGlobals.buildIdentifier,
        sdkVersion: sdkGlobals.sdkVersion,
        sdkDate: sdkGlobals.sdkVersion.split(".")[2],
        sdkYear: sdkGlobals.sdkVersion.split(".")[2].substr(0, 2),
        getFormattedCallDescription: getFormattedCallDescription,
        globalPrefix: globalPrefix,
        getCallDoc: getCallDoc,
        vsVer: "v141", // As C++ versions change, we may need to update this
        vsYear: "2017", // As VS versions change, we may need to update this
        winSdkVersion: "10.0.17763.0" // Which version of the Windows SDK (A VS installation option) to use
    };

    templatizeTree(locals, path.resolve(sourceDir, "source"), apiOutputDir);
    for (var a = 0; a < apis.length; a++) {
        makeApiFiles(apis[a], sourceDir, apiOutputDir);
    }

    var testXMLRefDocsJsonTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/XMLRefDocs.json.ejs"));
    writeFile(path.resolve(apiOutputDir, "test/", "XMLRefDocs.json.autogen"), testXMLRefDocsJsonTemplate(locals));

};

function makeApiFiles(api, sourceDir, apiOutputDir) {

    if (api.name === "Shared") {
        // Use just globalPrefix for shared datatypes
        var prefix = globalPrefix;
    } else {
        prefix = globalPrefix + api.name;
    }

    populateDictionaryEntryTypes(api);

    var locals = {
        api: api,
        prefix: prefix,
        categorizedApi: categorizedApis[api.name],
        categorizedApis: categorizedApis,
        enumtypes: getEnumTypes(api.datatypes),
        sortedClasses: getSortedClasses(api),
        getBaseTypes: getBaseTypes,
        getPropertyDefinition: getPropertyDefinition,
        getPropertyFromJson: getPropertyFromJson,
        addPropertyToJson: addPropertyToJson,
        getPropertyName: getPropertyName,
        canDefaultCopyConstructor: canDefaultCopyConstructor,
        getCopyConstructorInitializationList: getCopyConstructorInitializationList,
        getMoveConstructorInitializationList: getMoveConstructorInitializationList,
        getCopyConstructorBody: getCopyConstructorBody,
        addAuthHeader: addAuthHeader,
        isSerializable: isSerializable,
        isFixedSize: isFixedSize,
        getFormattedDatatypeDescription: getFormattedDatatypeDescription,
        getFormattedCallDescription: getFormattedCallDescription,
        getFormattedCallRemarks: getFormattedCallRemarks,
        getRequestExample: getRequestExample,
        getPublicPropertyType: getPublicPropertyType,
        testStatusMap: testStatusMap,
        apiGrouping: apiGrouping
    };

    var dataModelHeaderTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/_DataModels.h.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/source/" + api.name, api.name + "DataModels.h"), dataModelHeaderTemplate(locals));

    var dataModelTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/_DataModels.cpp.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/source/" + api.name, api.name + "DataModels.cpp"), dataModelTemplate(locals));

    var dataModelTemplate_c = getCompiledTemplate(path.resolve(sourceDir, "templates/PlayFab_DataModels.h.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/include/playFab", "PlayFab" + api.name + "DataModels.h"), dataModelTemplate_c(locals));

    // Currently don't need anything except data models for Shared API
    if (!(api.name === "Shared")) {

        var testCppTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/Test.cpp.ejs"));
        writeFile(path.resolve(apiOutputDir, "test/TestApp/AutoGenTests/", "AutoGen" + api.name + "Tests.cpp"), testCppTemplate(locals));

        var testLogCppTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/TestLog.cpp.ejs"));
        writeFile(path.resolve(apiOutputDir, "test/TestApp/AutoGenTests/", "AutoGen" + api.name + "TestLog.cpp"), testLogCppTemplate(locals));

        var testDataCppTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/TestData.cpp.ejs"));
        writeFile(path.resolve(apiOutputDir, "test/TestApp/AutoGenTests/", "AutoGen" + api.name + "TestData.cpp.autogen"), testDataCppTemplate(locals));

        var testHeaderTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/Test.h.ejs"));
        writeFile(path.resolve(apiOutputDir, "test/TestApp/AutoGenTests/", "AutoGen" + api.name + "Tests.h"), testHeaderTemplate(locals));

        var testCsvTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/API-List.csv.ejs"));
        writeFile(path.resolve(apiOutputDir, "test/", "API-List.csv"), testCsvTemplate(locals));

        var iapihTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/_Api.h.ejs"));
        writeFile(path.resolve(apiOutputDir, "code/source/" + api.name, api.name + "Api.h"), iapihTemplate(locals));

        var iapiCppTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/_Api.cpp.ejs"));
        writeFile(path.resolve(apiOutputDir, "code/source/" + api.name, api.name + "Api.cpp"), iapiCppTemplate(locals));

        var publicApiHeaderTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/PlayFab_Api.h.ejs"));
        writeFile(path.resolve(apiOutputDir, "code/include/playfab", "PlayFab" + api.name + "Api.h"), publicApiHeaderTemplate(locals));

        var publicApiTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/PlayFab_Api.cpp.ejs"));
        writeFile(path.resolve(apiOutputDir, "code/source/" + api.name, "PlayFab" + api.name + "Api.cpp"), publicApiTemplate(locals));

        if (locals.categorizedApi.loginCalls.length > 0) {
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

function jsonEscape(str)
{
    return str.replace(/\n/g, "").replace(/\r/g, "").replace(/\t/g, "").replace(/  /g, " ").replace(/  /g, " ");
}

function parseDataFile(filename) {
    const fs = require('fs');

    let fullPath = path.resolve(__dirname, filename);
    let rawdata = fs.readFileSync(fullPath, 'utf8');
    let dataEscaped = jsonEscape(rawdata);
    let data = JSON.parse(dataEscaped);
    return data;
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
                    var propertyDataType;
                    if (property.actualtype in datatypes) {
                        propertyDataType = datatypes[property.actualtype]
                    } else if (property.actualtype in sharedApi.datatypes) {
                        propertyDataType = sharedApi.datatypes[property.actualtype]
                    }
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
        if (!(apiCall.request in datatypes || apiCall.request in sharedApi.datatypes)) {
            apiCall.request = "void";
        }
        if (!(apiCall.result in datatypes || apiCall.result in sharedApi.datatypes)) {
            apiCall.result = "void";
        }
    }

}

// When adding a datatype to sharedApi.datatypes, ensure that the types are actually identical (have the same properties & property types)
function ensureTypesMatch(type1, type2) {
    if (type1.isenum) {
        if (type2.isenum) {
            return true;
        }
        else {
            return false;
        }
    }

    if (!(type1.properties.length === type2.properties.length)) {
        return false;
    }

    for (var i = 0; i < type1.properties.length; i++) {
        var property1 = type1.properties[i];
        var property2 = type2.properties[i];
        if (!(property1.name === property2.name && property1.actualtype === property2.actualtype && property1.optional === property2.optional)) {
            return false;
        }
    }
    return true
}

function populateSharedDatatypes(apis) {
    // Extract datatypes that are used by multiple APIs into sharedApi.dataypes.

    // Categorizing all LoginResult types (and their subtypes) as shared. This is because the LoginResult isn't returned directly to clients after a login call,
    // its stored as part of the returned Entity object. Client & Server Login results are identical (but have different service namespaces & typenames), and returning the
    // data from the Entity is much simpler if we unify those types. 

    // First rename all LoginResults & update all references to them
    var unifiedLoginResultName = "LoginResult";

    for (var i = 0; i < apis.length; i++) {
        var datatypes = apis[i].datatypes;
        for (var typename in datatypes) {
            if (typename.toLowerCase().endsWith(unifiedLoginResultName.toLowerCase())) {
                var datatype = datatypes[typename];
                if (!(typename === datatype.name)) {
                    throw Error("datatypes key & datatype.name don't match");
                }
                datatype.name = unifiedLoginResultName;
                delete datatypes[typename];
                datatypes[unifiedLoginResultName] = datatype;
            }
        }
        var calls = apis[i].calls;
        for (var ci = 0; ci < calls.length; ci++) {
            var call = calls[ci];
            if (call.result.toLowerCase().endsWith(unifiedLoginResultName.toLowerCase())) {
                call.result = unifiedLoginResultName;
            }
        }
    }

    // Now extract those types (and their subtypes) from each api.datatypes into sharedApi.datatypes 
    var extractType = function (typename) {
        console.log("extracting " + typename);
        for (var i = 0; i < apis.length; i++) {
            var apiDatatypes = apis[i].datatypes;
            if (apiDatatypes.hasOwnProperty(typename)) {
                if (sharedApi.datatypes.hasOwnProperty(typename)) {
                    if (!ensureTypesMatch(sharedApi.datatypes[typename], apiDatatypes[typename])) {
                        throw Error("Mismatch between sharedApi and " + apis[i].name + " " + typename);
                    }
                    delete apiDatatypes[typename];
                } else {
                    var apiDataype = apiDatatypes[typename];
                    if (apiDataype.properties) {
                        for (var propIdx = 0; propIdx < apiDataype.properties.length; propIdx++) {
                            var property = apiDataype.properties[propIdx];
                            if (property.isclass || property.isenum) {
                                extractType(property.actualtype);
                            }
                        }
                    }
                    sharedApi.datatypes[typename] = apiDataype;
                    delete apiDatatypes[typename];
                }
            }
        }
    }

    extractType(unifiedLoginResultName);
}

function categorizeCalls(apis) {
    // Categorize calls for each API into the following categories:
    // loginCalls: calls which acquire an AuthContext
    // otherCalls: calls which require an AuthContext (i.e. auth = "EntityToken" || "SessionTicket") OR a secretKey (i.e. auth = "SecretKey")
    //
    // Note whether a secret key is required for the "loginCalls" as well as for the "otherCalls". 
    // Annotate result types for auth methods as internal only. The public result of a login call will always be an Entity handle; the service response
    // model will not be directly returned to clients.

    for (var i = 0; i < apis.length; i++) {
        var api = apis[i];
        pruneEmptyTypes(api);
        var categorizedApi = { loginCalls: [], otherCalls: [] };

        for (var j = 0; j < api.calls.length; j++) {
            var call = api.calls[j];
            if (call.url === "/Authentication/GetEntityToken") {
                // Special case for GetEntityToken API. API spec marks it as Auth type "None", but it allows any of the auth tokens to be used.
                // In the SDK, we will split this into two APIs, one that can be called by an authenticated Entity, and one that can be called via
                // a PlayFabStateHandle w/ a SecretKey. The loginCall can be auto generated, the Entity call will be implemented manually 
                // as a one off. When getting an EntityToken using an EntityHandle, the requesting Entity must "own" the entity for which it
                // is requesting a token.

                call.auth = "SecretKey"; // Set auth to "SecretKey" so that the GetEntityToken login call is autogenerated correctly
                categorizedApi.loginCalls.push(call);
                categorizedApi.loginCallsRequireSecretKey = true;

                if (api.datatypes.hasOwnProperty(call.result)) {
                    api.datatypes[call.result]["isInternalOnly"] = true;
                } else if (sharedApi.datatypes.hasOwnProperty(call.result)) {
                    sharedApi.datatypes[call.result]["isInternalOnly"] = true;
                } else {
                    throw Error("Call result type not found");
                }

            } else if (call.result.toLowerCase().endsWith("loginresult") || call.result === "RegisterPlayFabUserResult") {
                categorizedApi.loginCalls.push(call);
                if (call.auth === "SecretKey") {
                    categorizedApi.loginCallsRequireSecretKey = true;
                }

                if (api.datatypes.hasOwnProperty(call.result)) {
                    api.datatypes[call.result]["isInternalOnly"] = true;
                } else if (sharedApi.datatypes.hasOwnProperty(call.result)) {
                    sharedApi.datatypes[call.result]["isInternalOnly"] = true;
                } else {
                    throw Error("Call result type not found");
                }

            } else if (call.auth === "EntityToken" || call.auth === "SessionTicket" || call.auth === "None") {
                categorizedApi.otherCalls.push(call);
                if (categorizedApi.otherCallsRequireSecretKey) {
                    throw Error("A single API can't require both Entity AuthTokens and a secretKey");
                }
                categorizedApi.otherCallsRequireSecretKey = false;
            } else if (call.auth === "SecretKey") {
                categorizedApi.otherCalls.push(call);
                if (categorizedApi.otherCallsRequireSecretKey === false) {
                    throw Error("A single API can't require both an Entity AuthTokens and a secretKey");
                }
                categorizedApi.otherCallsRequireSecretKey = true;
            } else {
                throw Error("Unable to categorize api call");
            }
        }

        categorizedApis[api.name] = categorizedApi;
    }
}

function getEnumTypes(datatypes) {
    var enumtypes = [];

    for (var typeIdx in datatypes) // Add all types and their dependencies
        if (datatypes[typeIdx].isenum)
            enumtypes.push(datatypes[typeIdx]);
    return enumtypes;
}

function getSortedClasses(api) {
    var sortedClasses = [];
    var addedTypes = new Set();

    var addType = function (datatype) {
        if (addedTypes.has(datatype.name) || datatype.isenum)
            return;
        // In C++, dependent types must be defined first
        if (datatype.properties) {
            for (var propIdx = 0; propIdx < datatype.properties.length; propIdx++) {
                var property = datatype.properties[propIdx];
                if ((property.isclass || property.isenum) && api.datatypes.hasOwnProperty(property.actualtype)) {
                    addType(api.datatypes[property.actualtype]);
                }
            }
        }
        addedTypes.add(datatype.name);
        sortedClasses.push(datatype);
    };

    // Add all types and their dependencies
    for (var typeIdx in api.datatypes) {
        addType(api.datatypes[typeIdx]);
    }

    // Annotate classes
    annotateClasses(sortedClasses, api);

    return sortedClasses;
}

// Annotates request & result datatypes with the APIs they are used with for documentation purposes
function annotateClasses(sortedClasses, api) {
    for (var classIdx = 0; classIdx < sortedClasses.length; classIdx++) {
        var datatype = sortedClasses[classIdx];
        if (api.hasOwnProperty("calls")) {
            for (var callIdx = 0; callIdx < api.calls.length; callIdx++) {
                var call = api.calls[callIdx];
                if (call.request === datatype.name || call.result === datatype.name) {
                    if (datatype.calls) {
                        datatype.calls.push(call.name);
                    } else {
                        datatype.calls = [];
                        datatype.calls.push(call.name);
                    }
                }
            }
        }
    }
}

function populateDictionaryEntryTypes(api) {
    // Gets all (non-primitive) types which appear as a Value in a map

    if (!api.hasOwnProperty("dictionaryEntryTypes")) {
        api["dictionaryEntryTypes"] = {};
    }

    for (var typeIdx in api.datatypes) {
        var datatype = api.datatypes[typeIdx];
        if (datatype.properties) {
            for (var propIdx = 0; propIdx < datatype.properties.length; propIdx++) {
                var property = datatype.properties[propIdx];
                if (property.collection === "map" && (property.isclass || property.isenum)) {
                    if (sharedApi.datatypes.hasOwnProperty(property.actualtype) && !sharedApi.dictionaryEntryTypes.hasOwnProperty(property.actualtype)) {
                        sharedApi.dictionaryEntryTypes[property.actualtype] = sharedApi.datatypes[property.actualtype];
                    } else if (api.datatypes.hasOwnProperty(property.actualtype) && !api.dictionaryEntryTypes.hasOwnProperty(property.actualtype)) {
                        api.dictionaryEntryTypes[property.actualtype] = api.datatypes[property.actualtype];
                    }
                }
            }
        }
    }
}

// *************************** ejs-exposed methods ***************************

function addAuthHeader(apiCall, tabbing) {
    switch (apiCall.auth) {
        case "EntityToken": {
            var output = ("auto& entityToken{ m_tokens->EntityToken() };\n" + tabbing + "if (!entityToken.token)\n" + tabbing + "{\n" + tabbing + "    return E_PLAYFAB_NOENTITYTOKEN;\n" + tabbing + "}\n");
            output += (tabbing + "headers.emplace(\"X-EntityToken\", entityToken.token);");
            return output;
        }
        case "SessionTicket": {
            output = ("auto sessionTicket{ m_tokens->SessionTicket() };\n" + tabbing + "if (sessionTicket.empty())\n" + tabbing + "{\n" + tabbing + "    return E_PLAYFAB_NOSESSIONTICKET;\n" + tabbing + "}\n");
            output += (tabbing + "headers.emplace(\"X-Authorization\", std::move(sessionTicket));");
            return output;
        }
        case "SecretKey": {
            output = ("if (m_secretKey == nullptr || m_secretKey->empty())\n" + tabbing + "{\n" + tabbing + "    return E_PLAYFAB_NOSECRETKEY;\n" + tabbing + "}\n");
            output += (tabbing + "headers.emplace(\"X-SecretKey\", *m_secretKey);");
            return output;
        }
        case "None": {
            return "// No auth header required for this API";
        }
        default: {
            throw Error("getAuthParams: Unknown auth type: " + apiCall.auth + " for " + apiCall.name);
        }
    }
}

// Returns whether the C++ model for a datatype is fixed size
function isFixedSize(datatype) {
    for (var i = 0; i < datatype.properties.length; i++) {
        if (requiresDynamicStorage(datatype.properties[i])) {
            return false;
        }
    }
    return true;
}

// Returns whether the C++ model for a datatype can be (trivially) serialized into a byte buffer
function isSerializable(datatype) {
    // For now, calling a datatype serializable if the only extra memory it needs is for Strings. This avoids have to deal with
    // alignment issues, nested classes, and collections.
    // Might be possible to relax these requirements if we really wanted to.

    for (var i = 0; i < datatype.properties.length; i++) {
        var property = datatype.properties[i];
        if (requiresDynamicStorage(property)) {
            if (!(property.actualtype === "String" && !property.collection)) {
                return false;
            }
        }
    }
    return true;
}

function getBaseTypes(prefix, datatype) {
    var baseTypes = "";

    if (!datatype.isInternalOnly) {
        baseTypes = "public " + prefix + datatype.name + ", ";
    }

    if (isSerializable(datatype) || isFixedSize(datatype)) {
        baseTypes += "public SerializableModel";
    } else {
        baseTypes += "public BaseModel";
    }

    if (datatype.className.toLowerCase().endsWith("response") || datatype.className.toLowerCase().endsWith("result")) {
        baseTypes += ", public ApiResult";
    }

    return baseTypes;
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

function requiresDynamicStorage(property) {
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

    // If property type is shared, override 'prefix' with the global prefix
    if (sharedApi.datatypes.hasOwnProperty(property.actualtype)) {
        prefix = globalPrefix;
    }

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
    if (!(property.actualtype === "object")) {
        if (property.collection === "map") {
            if (property.isclass) {
                return "AssociativeArrayModel<" + getDictionaryEntryTypeFromValueType(prefix + type) + ", " + type + ">";
            } else if (type === "String") {
                return "AssociativeArrayModel<PlayFabStringDictionaryEntry, String>";
            } else {
                return "AssociativeArrayModel<" + getDictionaryEntryTypeFromValueType(type) + ", void>";
            }
        } else if (property.collection === "array") {
            if (property.isclass) {
                return "PointerArrayModel<" + prefix + type + ", " + type + ">";
            } else if (type === "String") {
                return "PointerArrayModel<char, String>";
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

function getPublicPropertyType(property, prefix, addConsts) {
    var type = "";

    // If property type is shared, override 'prefix' with the global prefix
    if (sharedApi.datatypes.hasOwnProperty(property.actualtype)) {
        prefix = globalPrefix;
    }

    // Service types that can be mapped directly to C types
    var types = {
        "String": "const char*", "Boolean": "bool", "int16": "int16_t", "uint16": "uint16_t", "int32": "int32_t", "uint32": "uint32_t",
        "int64": "int64_t", "uint64": "uint64_t", "float": "float", "double": "double", "DateTime": "time_t", "object": "PlayFabJsonObject"
    };

    if (property.actualtype in types) {
        type = types[property.actualtype];
    } else if (property.isclass || property.isenum) {
        type = prefix + property.actualtype;
    } else {
        throw Error("Unrecognized property type " + property.actualtype);
    }

    // By design class properties are always pointers. Pointers will ultimately point to derived C++ internal Objects which
    // can automatically manage their cleanup & copying via destructors and copy constructors

    if (addConsts === false) {
        if (!(property.actualtype === "object")) {
            if (property.collection === "map") {
                // array of dictionary entries
                return getDictionaryEntryTypeFromValueType(type);
            } else if (property.collection === "array") {
                if (property.isclass) {
                    // array of pointers
                    return type;
                } else {
                    return type;
                }
            } else if (property.optional) {
                // Types which aren't already nullable will be made pointer types
                if (!(type === "const char*" || type === "PlayFabJsonObject")) {
                    return type;
                }
            }
        }

        if (property.isclass) {
            return type;
        }
    }
    else {
        // Add type modifications depending on "collection" & "optional" attributes
        if (!(property.actualtype === "object")) {
            if (property.collection === "map") {
                // array of dictionary entries
                return "struct " + getDictionaryEntryTypeFromValueType(type) + " const*";
            } else if (property.collection === "array") {
                if (property.isclass) {
                    // array of pointers
                    return type + " const* const*";
                } else {
                    return type + " const*";
                }
            } else if (property.optional) {
                // Types which aren't already nullable will be made pointer types
                if (!(type === "const char*" || type === "PlayFabJsonObject")) {
                    return type + " const*";
                }
            }
        }

        if (property.isclass) {
            return type + " const*";
        }
    }

    return type;
}

function getPropertyName(property, isPrivate) {
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

    // Special case for some property names
    if (name.startsWith("pSN")) {
        name = "P" + name.slice(1);
    } else if (name.startsWith("mAC")) {
        name = "M" + name.slice(1);
    } else if (name.startsWith("iPAddress")) {
        name = "I" + name.slice(1);
    } else if (name.startsWith("oS")) {
        name = "O" + name.slice(1);
    }

    return isPrivate ? "m_" + name : name;
}

function getPropertyDefinition(tabbing, datatype, property, prefix, isInternal) {
    var output = "";

    // for public properties, add XML ref doc comments
    if (!isInternal) {
        var xmlComment = tabbing + "/// <summary>\n" + tabbing + "///"
        var propertyDescription = property.optional ? "(Optional) " : "";
        if (property.description) {
            propertyDescription += property.description.charAt(0).toUpperCase() + property.description.slice(1);
            if (!propertyDescription.endsWith(".")) {
                propertyDescription += ".";
            }
        } else {
            propertyDescription += (property.name + " property.");
        }

        xmlComment = appendToXmlDocComment(xmlComment, propertyDescription);
        xmlComment += ("\n" + tabbing + "/// </summary>")
        output += ("\n" + xmlComment);
    }

    if (!isInternal || datatype.isInternalOnly || requiresDynamicStorage(property)) {
        var type = isInternal ? getInternalPropertyType(property, prefix) : getPublicPropertyType(property, prefix, true);
        var propName = getPropertyName(property, datatype.isInternalOnly ? false : isInternal);
        output += ("\n" + tabbing + type + " " + propName + ";");

        // For public collection properties add an additional "count" property
        if (property.collection && !(type === "PlayFabJsonObject") && !isInternal) {
            output += ("\n\n" + tabbing + "/// <summary>\n" + tabbing + "/// Count of " + propName + "\n" + tabbing + "/// </summary>");
            output += ("\n" + tabbing + "uint32_t " + propName + "Count;");
        }
    }

    if (!isInternal) {
        output += "\n";
    }

    return output;
}

function getPropertyFromJson(property, datatype) {
    var publicPropName = getPropertyName(property, false);
    var privatePropName = getPropertyName(property, true);

    var output = "JsonUtils::ObjectGetMember(input, \"" + property.name + "\", ";

    if (requiresDynamicStorage(property) && !datatype.isInternalOnly) {
        // if there is internal storage for a property, get the internal value and the public pointer to it
        output += (privatePropName + ", " + publicPropName);
    } else {
        output += publicPropName;
    }

    if (property.collection && !(property.actualtype === "object") && !datatype.isInternalOnly) {
        // for collections, also retreive the collection count
        output += (", " + publicPropName + "Count");
    }

    // for DateTime values, set optional param "convertToIso8601String" to true
    if (property.actualtype === "DateTime") {
        output += ", true";
    }

    return output + ");";
}

function addPropertyToJson(property, datatype) {
    var publicPropName = getPropertyName(property, false);

    var output = "JsonUtils::ObjectAddMember(output, \"" + property.name + "\", " + (!datatype.isInternalOnly ? "input." : "") + publicPropName;

    // for collections, pass the collection count
    if (property.collection && !(property.actualtype === "object") && !datatype.isInternalOnly) {
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
            if (requiresDynamicStorage(property)) {
                return false;
            }
        }
    }
    return true;
}

function getCopyConstructorInitializationList(tabbing, datatype, prefix) {
    var output = "";
    var conjunction = " :\n" + tabbing;

    if (!(datatype.isInternalOnly)) {
        output += conjunction + prefix + datatype.name + "{ src }"
        conjunction = ",\n" + tabbing;
    }

    for (var propIdx = 0; propIdx < datatype.properties.length; propIdx++) {
        var property = datatype.properties[propIdx];
        if (datatype.isInternalOnly || requiresDynamicStorage(property)) {
            var propName = getPropertyName(property, datatype.isInternalOnly ? false : true);
            output += (conjunction + propName + "{ src." + propName + " }");
            conjunction = ",\n" + tabbing;
        }
    }
    return output;
}

function getMoveConstructorInitializationList(tabbing, datatype, prefix) {
    var output = "";
    var conjunction = " :\n" + tabbing;

    if (!(datatype.isInternalOnly)) {
        output += conjunction + prefix + datatype.name + "{ src }"
        conjunction = ",\n" + tabbing;
    }

    for (var propIdx = 0; propIdx < datatype.properties.length; propIdx++) {
        var property = datatype.properties[propIdx];
        if (datatype.isInternalOnly || requiresDynamicStorage(property)) {
            var propName = getPropertyName(property, datatype.isInternalOnly ? false : true);
            output += (conjunction + propName + "{ std::move(src." + propName + ") }");
            conjunction = ",\n" + tabbing;
        }
    }
    return output;
}

function getCopyConstructorBody(tabbing, datatype, prefix) {
    var output = "";
    for (var propIdx = 0; propIdx < datatype.properties.length; propIdx++) {
        var property = datatype.properties[propIdx];
        if (requiresDynamicStorage(property) && !datatype.isInternalOnly) {
            var publicPropName = getPropertyName(property, false);
            var privatePropName = getPropertyName(property, true);
            var internalPropertyType = getInternalPropertyType(property, prefix);

            if (internalPropertyType === "JsonObject") {
                output += ("\n" + tabbing + publicPropName + ".stringValue = " + privatePropName + ".StringValue();");
            } else if (internalPropertyType.includes("Array")) {
                output += ("\n" + tabbing + publicPropName + " = " + privatePropName + ".Empty() ? nullptr : " + privatePropName + ".Data();");
            } else if (internalPropertyType === "String" || internalPropertyType.includes("Vector")) {
                output += ("\n" + tabbing + publicPropName + " = " + privatePropName + ".empty() ? nullptr : " + privatePropName + ".data();");
            } else if (property.optional) {
                output += ("\n" + tabbing + publicPropName + " = " + privatePropName + " ? " + privatePropName + ".operator->() : nullptr;");
            } else if (property.isclass) {
                output += ("\n" + tabbing + publicPropName + " = (" + getPublicPropertyType(property, prefix, true) + ")&" + privatePropName + ";");
            } else {
                throw Error("Unable to copy property of type " + property.actualtype);
            }
        }
    }
    return output;
}

// appends str to existing XmlDoc comment, ensuring proper line breaks and formatting
function appendToXmlDocComment(comment, str) {
    var tabbing = comment.slice(0, comment.indexOf("///"));
    var curLineLength = comment.length - comment.lastIndexOf("\n");
    var words = str.split(" ");
    for (var i = 0; i < words.length; i++) {
        if (curLineLength > 100) {
            comment += ("\n" + tabbing + "///");
            curLineLength = tabbing.length + 3;
        }
        comment += (" " + words[i]);
        curLineLength += (words[i].length + 1);
    }
    return comment;
}

function getFormattedDatatypeDescription(prefix, datatype) {
    var output = "/// " + prefix + datatype.name + " data model.";

    if (datatype.description) {
        if (datatype.description.endsWith(".")) {
            output = appendToXmlDocComment(output, datatype.description);
        } else {
            output = appendToXmlDocComment(output, datatype.description + ".");
        }
    }

    if (datatype.hasOwnProperty("calls")) {
        var callUsage = ""; // addition info on which API calls the struct is used in
        if (datatype.isRequest) {
            callUsage += "Request object for";

        } else if (datatype.isResult) {
            callUsage += "Result payload for";
        }
        if (callUsage.length > 0) {
            for (i = 0; i < datatype.calls.length; i++) {
                // ensure proper grammer in call usage list
                if (i > 1 && i === datatype.calls.length - 1) { // Used in at least 3 calls and this is the last one
                    callUsage += ", and";
                } else if (i === 1 && datatype.calls.length === 2) { // Used in 2 calls and this is the second one
                    callUsage += " and";
                } else if (i > 0) {
                    callUsage += ",";
                }
                callUsage += (" " + prefix + datatype.calls[i] + "Async");
            }
            output = appendToXmlDocComment(output, callUsage + ".");
        }
    }

    return output;
}

function getSeeAlso(call) {
    if (call.seeAlso == undefined || call.seeAlso.length == 0) {
        return "";
    }

    var result = "See also ";
    for (var i in call.seeAlso) {
        result += globalPrefix + call.seeAlso[i].replace(/"/g, "'").replace("/", "") + "Async";
        if (i != call.seeAlso.length - 1) {
            result += ", ";
        }
    }

    return result;
}

function getCallDoc(apiName, call) {
    var asyncName = globalPrefix + apiName + call.name + "Async";
    if (asyncName in xmlRefDocs) {
        var docForCall = xmlRefDocs[asyncName];
        return docForCall;
    }

    var doc = {};
    if (call.summary === undefined || call.summary.length == 0) {
        doc.summary = "TODO";
    }
    else {
        doc.summary = call.summary.replace(/"/g, "'");
    }

    var remarks = (call.requestDetails !== undefined) ? call.requestDetails.replace(/"/g, "'") : "";
    var seeAlso = getSeeAlso(call);
    if (remarks.length > 0 && seeAlso.length > 0) {
        remarks += " ";
    }
    remarks += seeAlso;
    if (remarks.length > 0) {
        doc.remarks = remarks;
    }
    return doc;
}

function getFormattedCallDescription(apiName, call) {
    return appendToXmlDocComment("///", getCallDoc(apiName, call).summary);
}

function getFormattedCallRemarks(apiName, call) {
    var callDoc = getCallDoc(apiName, call);
    if (callDoc.remarks !== undefined) {
        return appendToXmlDocComment("///", callDoc.remarks);
    }
    else if (call.result === "void" || call.url === "/Authentication/GetEntityToken")
    {
        return "/// Call <see cref=\"XAsyncGetStatus\"/> to get the status of the operation."
    }
    else
    {
        var getResultName = globalPrefix + apiName + call.name + "GetResult";
        return "/// If successful, call <see cref=\"" + getResultName + "\"/> to get the result."
    }
}

function jsonEscapeQuotes(input) {
    if (input !== null) {
        input = input.replace(/"/g, "\\\"");
    }
    return input;
}

function getCorrectedRequestExample(api, apiCall) {
    var output = JSON.parse(apiCall.requestExample);
    checkReplacements(api, output);
    return "\"" + jsonEscapeQuotes(jsonEscape(JSON.stringify(output, null, 2))) + "\"";
}

function doReplace(obj, paramName, newValue) {
    if (obj.hasOwnProperty(paramName)) {
        console.log("Replaced: " + obj[paramName] + " with " + newValue);
        obj[paramName] = newValue;
    }
};

function checkReplacements(api, obj) {
    for (var replaceCategory in propertyReplacements) {
        if (replaceCategory === "generic") {
            for (var genReplaceName1 in propertyReplacements[replaceCategory]) {
                doReplace(obj, genReplaceName1, propertyReplacements[replaceCategory][genReplaceName1]);
            }
        }
        if (replaceCategory === api.name) {
            for (var apiReplaceName in propertyReplacements[replaceCategory]) {
                if (apiReplaceName === "generic") {
                    for (var genReplaceName2 in propertyReplacements[replaceCategory][apiReplaceName]) {
                        doReplace(obj, genReplaceName2, propertyReplacements[replaceCategory][apiReplaceName][genReplaceName2]);
                    }
                }
                doReplace(obj, apiReplaceName, propertyReplacements[replaceCategory][apiReplaceName]);
            }
        }
    }
}

function getRequestExample(api, apiCall) {
    var msg = null;
    if (apiCall.requestExample.length > 0 && apiCall.requestExample.indexOf("{") >= 0) {
        if (apiCall.requestExample.indexOf("\\\"") === -1) { // I can't handle json in a string in json in a string...
            return getCorrectedRequestExample(api, apiCall);
        } else {
            msg = "CANNOT PARSE EXAMPLE BODY: ";
        }
    }

    var props = api.datatypes[apiCall.request].properties;
    var output = {};
    for (var p = 0; p < props.length; p++) {
        output[props[p].name] = props[p].jsontype;
    }

    if (msg === null) {
        msg = "AUTO GENERATED BODY FOR: ";
    }
    console.log(msg + api.name + "." + apiCall.name);
    // console.log("    " + JSON.stringify(output, null, 2));
    return "\"" + jsonEscapeQuotes(jsonEscape(JSON.stringify(output, null, 2))) + "\"";;
}


