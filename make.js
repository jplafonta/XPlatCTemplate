var path = require("path");
const { fileURLToPath } = require("url");

// Making resharper less noisy - These are defined in Generate.js
if (typeof getCompiledTemplate === "undefined") getCompiledTemplate = function () { };
if (typeof templatizeTree === "undefined") templatizeTree = function () { };

// Global prefix for all SDK public symbols
var globalPrefix = "PF"; 

// Calls and datatypes produced based on api.calls & api.datatypes, but rather than being categorized by service API, they are
// categorized based on feature group. This structure will be used to generate the SDK.
var SDKFeatureGroups = {};

// Object describing SDK Customizations. This structure allows overriding the SDK featureGroup and/or platform for api calls.
//
// By default calls / datatypes will be categorized into SDKFeatureGroups based on service "subgroup"(api.calls[*].subgroup).
// customizations.featureGroupOverrides gives more control over the SDK feature groups we expose and which calls appear in which header file.
// The overrides will processed in order calls->apis->subgroup (if a call matches multiple overrides, the first match will be used).
//
// By default, each api call will be enabled on all platforms. customizations.platformExclusions allows excluding individual calls, full service
// apis, or full SDK feature groups on specific platforms. For example, if customizations.platformExclusions.apis.Admin = [HC_PLATFORM_GDK], the
// entire Admin service API set will be exluded for HC_PLATFORM_GDK.
//
// customizations object loaded from customizations.json.
var customizations = {};

// Used to generate XML header doc comments. Loaded from XMLRefDocs.json
var xmlRefDocs = {};

// Test related data structures
var prerequisiteApis = {};
var propertyReplacements = {};
var testStatusMap = {};

exports.makeCombinedAPI = function (apis, sourceDir, apiOutputDir) {
    console.log("Generating Combined api from: " + sourceDir + " to: " + apiOutputDir);

    try {
        propertyReplacements = require(path.resolve(sourceDir, "replacements.json"));
    } catch (ex) {
        throw "The file: replacements.json was not properly formatted JSON";
    }

    customizations = parseDataFile("customizations.json");
    xmlRefDocs = parseDataFile("XMLRefDocs.json");
    testStatusMap = parseDataFile("TestStatus.json");

    // Make SDK customizations to apis structure
    curateServiceApis(apis)

    // populate SDKFeatureGroups structure
    populateSDKFeatureGroups(apis);

    // Configure test prerequisites
    setPrerequisiteCalls(apis);

    var locals = {
        apis: apis,
        prerequisiteApis: prerequisiteApis,
        prefix: globalPrefix,
        SDKFeatureGroups: SDKFeatureGroups,
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
    for (var featureGroupName in SDKFeatureGroups) {
        makeFeatureGroupFiles(SDKFeatureGroups[featureGroupName], sourceDir, apiOutputDir);
    }

    var testXMLRefDocsJsonTemplate = getCompiledTemplate(path.resolve(sourceDir, "templates/XMLRefDocs.json.ejs"));
    writeFile(path.resolve(apiOutputDir, "test/", "XMLRefDocs.json.autogen"), testXMLRefDocsJsonTemplate(locals));
};

function makeFeatureGroupFiles(featureGroup, sourceDir, apiOutputDir) {

    if (featureGroup.name === "Shared") {
        // Use just globalPrefix for shared datatypes
        var prefix = globalPrefix;
    } else {
        prefix = globalPrefix + featureGroup.name;
    }

    var locals = {
        featureGroup: featureGroup,
        prefix: prefix,
        globalPrefix: globalPrefix,
        prerequisiteApis: prerequisiteApis[featureGroup.name],
        platformExclusions: customizations.platformExclusions,
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
    };

    // DataModels
    var publicDataModelsHeader = getCompiledTemplate(path.resolve(sourceDir, "templates/PFDataModels.h.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/include/playFab", globalPrefix + featureGroup.name + "DataModels.h"), publicDataModelsHeader(locals));

    var internalDataModelsHeader = getCompiledTemplate(path.resolve(sourceDir, "templates/DataModels.h.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/source/" + featureGroup.name, featureGroup.name + "DataModels.h"), internalDataModelsHeader(locals));

    var internalDataModels = getCompiledTemplate(path.resolve(sourceDir, "templates/DataModels.cpp.ejs"));
    writeFile(path.resolve(apiOutputDir, "code/source/" + featureGroup.name, featureGroup.name + "DataModels.cpp"), internalDataModels(locals));

    // Currently don't need anything except data models for Shared API
    if (featureGroup.name !== "Shared") {

        // Internal APIs
        var internalApisHeader = getCompiledTemplate(path.resolve(sourceDir, "templates/Calls.h.ejs"));
        writeFile(path.resolve(apiOutputDir, "code/source/" + featureGroup.name, featureGroup.name + ".h"), internalApisHeader(locals));

        var internalApis = getCompiledTemplate(path.resolve(sourceDir, "templates/Calls.cpp.ejs"));
        writeFile(path.resolve(apiOutputDir, "code/source/" + featureGroup.name, featureGroup.name + ".cpp"), internalApis(locals));

        // Public APIs
        var publicApisHeader = getCompiledTemplate(path.resolve(sourceDir, "templates/PFCalls.h.ejs"));
        writeFile(path.resolve(apiOutputDir, "code/include/playfab", globalPrefix + featureGroup.name + ".h"), publicApisHeader(locals));

        var publicApis = getCompiledTemplate(path.resolve(sourceDir, "templates/PFCalls.cpp.ejs"));
        writeFile(path.resolve(apiOutputDir, "code/source/" + featureGroup.name, globalPrefix + featureGroup.name + ".cpp"), publicApis(locals));

        // Test files
        var testHeader = getCompiledTemplate(path.resolve(sourceDir, "templates/Test.h.ejs"));
        writeFile(path.resolve(apiOutputDir, "test/TestApp/AutoGenTests/", "AutoGen" + featureGroup.name + "Tests.h"), testHeader(locals));

        var testMain = getCompiledTemplate(path.resolve(sourceDir, "templates/Test.cpp.ejs"));
        writeFile(path.resolve(apiOutputDir, "test/TestApp/AutoGenTests/", "AutoGen" + featureGroup.name + "Tests.cpp"), testMain(locals));

        var testLogging = getCompiledTemplate(path.resolve(sourceDir, "templates/TestLog.cpp.ejs"));
        writeFile(path.resolve(apiOutputDir, "test/TestApp/AutoGenTests/", "AutoGen" + featureGroup.name + "TestLog.cpp"), testLogging(locals));

        var testData = getCompiledTemplate(path.resolve(sourceDir, "templates/TestData.cpp.ejs"));
        writeFile(path.resolve(apiOutputDir, "test/TestApp/AutoGenTests/", "AutoGen" + featureGroup.name + "TestData.cpp.autogen"), testData(locals));
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

// Curate provided 'apis' object as follows:
// - Prune datatypes from apis[*].datatypes which are empty classes and update any references to them
// - Add a reference to the 'datatype' object for each 'call' object in apis[*].calls. This allows us to skip looking up the datatype by name later
// - Add a reference to the 'datatype' object for each 'property' object in apis[*].datatypes.*.properties.
// - Annotate each 'call' in apis[*].calls with "entityReturned=true" or "entityRequired=true" if the call returns/requires an SDK Entity object
function curateServiceApis(apis) {

    for (var apiIndex = 0; apiIndex < apis.length; apiIndex++) {
        var api = apis[apiIndex];
        var datatypes = api.datatypes;

        // Prune datatypes which are empty classes
        for (typename in datatypes) {
            datatype = datatypes[typename];
            if (datatype.properties && datatype.properties.length === 0) {
                delete datatypes[typename];
            }
        }

        // Update datatype.properties
        for (var typename in datatypes) {
            var datatype = datatypes[typename];
            if (datatype.properties) {
                for (var propertyIndex = datatype.properties.length - 1; propertyIndex >= 0; propertyIndex--) {
                    var property = datatype.properties[propertyIndex];

                    if (property.isclass) {
                        if (datatypes.hasOwnProperty(property.actualtype)) {
                            // If the datatype exists still (wasn't pruned above), add a reference to it in the 'property' for easy access
                            property.datatype = datatypes[property.actualtype];
                        } else {
                            // Otherwise remove the property
                            datatype.properties.splice(propertyIndex, 1);
                        }
                    } else if (property.isenum) {
                        property.datatype = datatypes[property.actualtype];
                    }
                }
            }
        }

        // Update calls
        var calls = api.calls;
        for (var callIndex = 0; callIndex < api.calls.length; callIndex++) {
            var call = calls[callIndex];

            if (call.request in datatypes) {
                call.requestDatatype = datatypes[call.request];
            } else {
                delete call.request;
            }

            if (call.result in datatypes) {
                call.resultDatatype = datatypes[call.result];
            } else {
                delete call.result;
            }

            // Note which PlayFab Entity object is required to make the call.
            if (call.auth === "SessionTicket") {
                call.entityRequired = "TitlePlayer"
            } else if (call.auth === "EntityToken") {
                call.entityRequired = "Entity";
            }

            // Note whether a PlayFab::Entity object is created (client side only) as a result of the call. These are mostly Login* methods.
            // Annotate result types for these calls as internal only. The public result of a login call will always be an Entity handle; the service response
            // model will not be directly returned to clients.
            if (call.url === "/Authentication/GetEntityToken") {
                // Special case for GetEntityToken call. Service API spec marks it as Auth type "None", but in reality authentication is required 
                // and any of the three auth tokens can be used. In the SDK, we will split this call into two APIs, one that can be called by an 
                // authenticated Entity, and one that can be called via a PFStateHandle w/ a SecretKey. The latter will be auto generated,
                // and the former will be implemented manually as a one off.
                // Note that when getting an EntityToken using an EntityHandle, the requesting Entity must "own" the entity for which it
                // is requesting a token.

                call.auth = "SecretKey"; // Set auth to "SecretKey" so that the GetEntityToken login call is autogenerated correctly
                call.entityReturned = "Entity";
                call.resultDatatype.isInternalOnly = true;

            } else if (call.result && (call.resultDatatype.name.endsWith("LoginResult") || call.resultDatatype.name === "RegisterPlayFabUserResult")) {
                call.entityReturned = "TitlePlayer";
                call.resultDatatype.isInternalOnly = true;
            }
        }
    }
}

// Compare two datatypes. If the types have the same type (enum vs class), the same name (datatype.name), and the same properties (datatype.properties).
// we can consider them the same in the SDK. If properties are optional in one type but not the other, consider them as always required.
// If the types meet these match requirements, deep copy each property so they are completely identical (This is needed because calls/properties still may reference
// either instance).
function doTypesMatch(type1, type2) {

    if (type1.name !== type2.name) {
        return false;
    }

    if (type1.isenum) {
        if (type2.isenum) {
            if (type1.enumvalues.length !== type2.enumvalues.length) {
                return false;
            }
            for (let i = 0; i < type1.enumvalues.length; i++) {
                if (type1.enumvalues[i].name !== type2.enumvalues[i].name) {
                    return false;
                }
            }
            return true;
        }
        else {
            return false;
        }
    }
    else {
        if (!(type1.properties.length === type2.properties.length)) {
            return false;
        }

        for (var i = 0; i < type1.properties.length; i++) {
            let property1 = type1.properties[i];
            let property2 = type2.properties[i];
            if (!(property1.name === property2.name && property1.actualtype === property2.actualtype)) {
                return false;
            }
        }

        // At this point we can consider the types equivalent, reconcile property optional
        for (let i = 0; i < type1.properties.length; i++) {
            let property1 = type1.properties[i];
            let property2 = type2.properties[i];
            if (property1.optional === false || property2.optional === false) {
                property1.optional = false;
                property2.optional = false;
            }
        }
    }

    return true;
}

// Add 'datatype' to featureGroup.datatypes. If the type already exists in a different featureGroup,
// remove it from that featureGroup and add the type to SDKFeatureGroups.shared.datatypes instead (rather than to 'featureGroup').
// Also annotate datatype with appropriate prefix based on featureGroup.name & isShared flag if the datatype is used within multiple SDKFeatureGroups
function addDatatypeToFeatureGroup(datatype, featureGroup) {

    var featureGroupDatatypes = featureGroup.datatypes;
    var sharedDatatypes = SDKFeatureGroups.Shared.datatypes;

    if (featureGroupDatatypes.hasOwnProperty(datatype.name)) {
        let existingDatatype = featureGroupDatatypes[datatype.name];

        if (existingDatatype == datatype) {
            // This datatype instance has already been added. No work to do
        } else if (doTypesMatch(datatype, existingDatatype)) {
            // Apply previously added annotations (prefix, etc.) to datatype
            Object.assign(datatype, existingDatatype);
        } else {
            // Use service API name to differtiate non-matching datatypes with the same name. Serivce namespace for data models have format "PlayFab.<ServiceApi>.Models".
            // Extract ServiceApi and mangle change datatype.name to <ServiceApi><datatype.name>. Note that this isn't just a public prefix,
            // it affects both the internal and public name for the datatype.
            if (existingDatatype.name === datatype.name) {
                existingDatatype.name = existingDatatype.classNameSpace.split(".")[1] + existingDatatype.name;
            }
            datatype.name = datatype.classNameSpace.split(".")[1] + datatype.name;
            datatype.prefix = globalPrefix + featureGroup.name;
            featureGroupDatatypes[datatype.name] = datatype;
            // existingDatatype intentionally remains indexed by service name (not mangled name) so that future type collisions will still be detected
        }
    } else if (sharedDatatypes.hasOwnProperty(datatype.name)) {

        let existingDatatype = sharedDatatypes[datatype.name];

        if (existingDatatype == datatype) {
            // This datatype instance has already been added. No work to do
        } else if (doTypesMatch(datatype, existingDatatype)) {
            // Update datatype with any annotations we've added previously (publicName, isShared, etc.).
            Object.assign(datatype, existingDatatype);
        } else {
            // Can't easily handle this situation so throw error here
            throw Error("Unexpected: Type mismatch cannot be reconciled");
        }

    } else {
        // Tentatively add datatype to featureGroup[datatype]. Will be moved to SDKFeatureGroups.Shared below if we discover that it is also referenced in another featureGroup
        datatype.prefix = globalPrefix + featureGroup.name;
        featureGroupDatatypes[datatype.name] = datatype;

        // Check if the a matching datatype already exists in another featureGroupName and move to sharedDatatypes if so
        for (var otherFeatureGroupName in SDKFeatureGroups) {
            if (otherFeatureGroupName !== featureGroup.name && otherFeatureGroupName !== "Shared") {
                var otherFeatureGroupDatatypes = SDKFeatureGroups[otherFeatureGroupName].datatypes;

                if (otherFeatureGroupDatatypes.hasOwnProperty(datatype.name)) {
                    let existingDatatype = otherFeatureGroupDatatypes[datatype.name];

                    if (doTypesMatch(datatype, existingDatatype)) {
                        datatype.prefix = globalPrefix;
                        datatype.isShared = true;
                        Object.assign(existingDatatype, datatype);
                        sharedDatatypes[datatype.name] = datatype;

                        // remove from the feature group datatypes now as well
                        delete otherFeatureGroupDatatypes[datatype.name];
                        delete featureGroupDatatypes[datatype.name];

                        break;
                    }
                }
            }
        }
    }

    // Recursively add all subtypes of 'datatype' as well
    if (datatype.hasOwnProperty("properties")) {
        for (var propertyIndex = 0; propertyIndex < datatype.properties.length; propertyIndex++) {
            var property = datatype.properties[propertyIndex];
            if (property.isclass || property.isenum) {
                addDatatypeToFeatureGroup(property.datatype, featureGroup);
            }
        }
    }
}

function getOrCreateFeatureGroup(name) {
    if (!SDKFeatureGroups.hasOwnProperty(name)) {
        SDKFeatureGroups[name] = {
            "name": name,
            "datatypes": {},
            "calls": [],
            "sortedClasses": [], // subset of 'datatypes' that are classes (datatype.isclass = true), sorted such that type dependencies are honored
            "dictionaryEntryTypes": {} // subset of 'datatypes' that appear as values in a map (property.collection = map)
        }
    }
    return SDKFeatureGroups[name];
}

function populateSDKFeatureGroups(apis) {

    // Create special "Shared" feature group manually. It will hold datatypes which are referenced in multiple SDKFeatureGroups.
    getOrCreateFeatureGroup("Shared");

    for (var apiIndex = 0; apiIndex < apis.length; apiIndex++) {
        var api = apis[apiIndex];
        var apiDatatypes = api["datatypes"];

        for (var callIndex = 0; callIndex < api.calls.length; ++callIndex) {

            var call = api.calls[callIndex];

            // Modify name of "Classic" API calls to avoid conflicts & clarify that they are classic APIs.
            // Note that this is in addition to the global & featureGroup prefixes that will be added to the public SDK APIs.
            if (api.name === "Client" || api.name === "Server" || api.name === "Admin") {
                call.name = api.name + call.name;
            }

            let featureGroupName = call.subgroup;
            let featureGroupOverrides = customizations.featureGroupOverrides;

            if (featureGroupOverrides.calls.hasOwnProperty(call.name)) {
                featureGroupName = featureGroupOverrides.calls[call.name];
            } else if (featureGroupOverrides.apis.hasOwnProperty(api.name)) {
                featureGroupName = featureGroupOverrides.apis[api.name];
            } else if (featureGroupOverrides.subgroups.hasOwnProperty(call.subgroup)) {
                featureGroupName = featureGroupOverrides.subgroups[call.subgroup];
            }

            let featureGroup = getOrCreateFeatureGroup(featureGroupName);
            featureGroup.calls.push(call);

            // Add request & response types to featureGroup.datatypes
            if (call.hasOwnProperty("requestDatatype")) {
                addDatatypeToFeatureGroup(call.requestDatatype, featureGroup);
            }
            if (call.hasOwnProperty("resultDatatype")) {
                addDatatypeToFeatureGroup(call.resultDatatype, featureGroup);
            }
        }
    }

    // populate featuresGroups.*.sortedClasses and SDKFeatureGroups.*.dictionaryEntryTypes
    for (var featureGroupName in SDKFeatureGroups) {
        var featureGroup = SDKFeatureGroups[featureGroupName]
        var sortedClasses = featureGroup.sortedClasses;
        var addedTypes = new Set();

        var addType = function (datatype) {
            if (addedTypes.has(datatype.name) || datatype.isenum) {
                return;
            }
            // In C++, dependent types must be defined first
            if (datatype.properties) {
                for (var propIdx = 0; propIdx < datatype.properties.length; propIdx++) {
                    var property = datatype.properties[propIdx];
                    if (property.isclass && featureGroup.datatypes.hasOwnProperty(property.datatype.name)) {
                        addType(property.datatype);
                    }
                }
            }
            addedTypes.add(datatype.name);
            sortedClasses.push(datatype);
        };

        // Add all types and their dependencies
        for (let typeKey in featureGroup.datatypes) {
            addType(featureGroup.datatypes[typeKey]);
        }

        // Add all (non-primitive) types which appear as a Value in a map featureGroup.dictionaryEntryTypes (or SDKFeatureGroups.Shared.dictionaryEntryTypes as appropriate)
        for (let typeKey in featureGroup.datatypes) {
            let datatype = featureGroup.datatypes[typeKey];
            if (datatype.properties) {
                for (var propertyIndex = 0; propertyIndex < datatype.properties.length; propertyIndex++) {
                    var property = datatype.properties[propertyIndex];
                    if (property.collection === "map" && (property.isclass || property.isenum)) {
                        if (property.datatype.isShared) {
                            SDKFeatureGroups.Shared.dictionaryEntryTypes[property.datatype.name] = property.datatype;
                        } else {
                            featureGroup.dictionaryEntryTypes[property.datatype.name] = property.datatype;
                        }
                    }
                }
            }
        }
    }
}

function setPrerequisiteCalls() {
    for (let featureGroupName in SDKFeatureGroups) {
        var featureGroup = SDKFeatureGroups[featureGroupName];
        prerequisiteApis[featureGroupName] = [];

        if (featureGroupName == "Groups") {
            prerequisiteApis[featureGroupName].push(featureGroup.calls.find(elem => elem.name === "CreateGroup"));
            prerequisiteApis[featureGroupName].push(featureGroup.calls.find(elem => elem.name === "GetGroup"));
            prerequisiteApis[featureGroupName].push(featureGroup.calls.find(elem => elem.name === "ApplyToGroup"));
            prerequisiteApis[featureGroupName].push(featureGroup.calls.find(elem => elem.name === "InviteToGroup"));
            prerequisiteApis[featureGroupName].push(featureGroup.calls.find(elem => elem.name === "BlockEntity"));
        }
    }
}

// *************************** ejs-exposed methods ***************************

// Used by functions which return code snippets
var tab = "    ";

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

function getBaseTypes(datatype) {
    var baseTypes = "";

    if (!datatype.isInternalOnly) {
        baseTypes = "public " + datatype.prefix + datatype.name + ", ";
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
        return globalPrefix + types[valueType] + "DictionaryEntry";
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

function getInternalPropertyType(property) {
    var type = "";

    // Service types that can be mapped directly to C++ types
    var types = {
        "String": "String", "Boolean": "bool", "int16": "int16_t", "uint16": "uint16_t", "int32": "int32_t", "uint32": "uint32_t",
        "int64": "int64_t", "uint64": "uint64_t", "float": "float", "double": "double", "DateTime": "time_t", "object": "JsonObject"
    };

    if (property.actualtype in types) {
        type = types[property.actualtype];
    } else if (property.datatype) {
        if (property.isclass) {
            type = property.datatype.name;
        } else if (property.isenum) {
            // Enums always defined in global namespace so add prefix
            type = property.datatype.prefix + property.datatype.name
        }
    } else {
        throw Error("Unrecognized property type " + property.actualtype);
    }

    // Modify type depending on collection & optional attributes. 
    if (!(property.actualtype === "object")) {
        if (property.collection === "map") {
            if (property.isclass) {
                return "AssociativeArrayModel<" + getDictionaryEntryTypeFromValueType(property.datatype.prefix + type) + ", " + type + ">";
            } else if (type === "String") {
                return "AssociativeArrayModel<" + globalPrefix + "StringDictionaryEntry, String>";
            } else {
                return "AssociativeArrayModel<" + getDictionaryEntryTypeFromValueType(type) + ", void>";
            }
        } else if (property.collection === "array") {
            if (property.isclass) {
                return "PointerArrayModel<" + property.datatype.prefix + type + ", " + type + ">";
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

function getPublicPropertyType(property, addConsts) {
    var type = "";

    // Service types that can be mapped directly to C types
    var types = {
        "String": "const char*", "Boolean": "bool", "int16": "int16_t", "uint16": "uint16_t", "int32": "int32_t", "uint32": "uint32_t",
        "int64": "int64_t", "uint64": "uint64_t", "float": "float", "double": "double", "DateTime": "time_t", "object": globalPrefix + "JsonObject"
    };

    if (property.actualtype in types) {
        type = types[property.actualtype];
    } else if (property.datatype) {
        if (!property.datatype.prefix) {
            throw Error("No prefix!");
        }
        type = property.datatype.prefix + property.datatype.name;
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
                if (!(type === "const char*" || type === (globalPrefix + "JsonObject"))) {
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
                if (!(type === "const char*" || type === (globalPrefix + "JsonObject"))) {
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

function getPropertyDefinition(datatype, property, isInternal) {
    var output = "";

    // for public properties, add XML ref doc comments
    if (!isInternal) {
        var xmlComment = tab + "/// <summary>\n" + tab + "///"
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
        xmlComment += ("\n" + tab + "/// </summary>")
        output += ("\n" + xmlComment);
    }

    if (!isInternal || datatype.isInternalOnly || requiresDynamicStorage(property)) {
        var type = isInternal ? getInternalPropertyType(property) : getPublicPropertyType(property, true);
        var propName = getPropertyName(property, datatype.isInternalOnly ? false : isInternal);
        output += ("\n" + tab + type + " " + propName + ";");

        // For public collection properties add an additional "count" property
        if (property.collection && !(type === (globalPrefix + "JsonObject")) && !isInternal) {
            output += ("\n\n" + tab + "/// <summary>\n" + tab + "/// Count of " + propName + "\n" + tab + "/// </summary>");
            output += ("\n" + tab + "uint32_t " + propName + "Count;");
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

function getCopyConstructorInitializationList(datatype) {
    var output = "";
    var conjunction = " :\n" + tab;

    if (!(datatype.isInternalOnly)) {
        output += conjunction + datatype.prefix + datatype.name + "{ src }"
        conjunction = ",\n" + tab;
    }

    for (var i = 0; i < datatype.properties.length; i++) {
        var property = datatype.properties[i];
        if (datatype.isInternalOnly || requiresDynamicStorage(property)) {
            var propName = getPropertyName(property, datatype.isInternalOnly ? false : true);
            output += (conjunction + propName + "{ src." + propName + " }");
            conjunction = ",\n" + tab;
        }
    }
    return output;
}

function getMoveConstructorInitializationList(datatype) {
    var output = "";
    var conjunction = " :\n" + tab;

    if (!(datatype.isInternalOnly)) {
        output += conjunction + datatype.prefix + datatype.name + "{ src }"
        conjunction = ",\n" + tab;
    }

    for (var i = 0; i < datatype.properties.length; i++) {
        var property = datatype.properties[i];
        if (datatype.isInternalOnly || requiresDynamicStorage(property)) {
            var propName = getPropertyName(property, datatype.isInternalOnly ? false : true);
            output += (conjunction + propName + "{ std::move(src." + propName + ") }");
            conjunction = ",\n" + tab;
        }
    }
    return output;
}

function getCopyConstructorBody(datatype) {
    var output = "";
    for (var propIdx = 0; propIdx < datatype.properties.length; propIdx++) {
        var property = datatype.properties[propIdx];
        if (requiresDynamicStorage(property) && !datatype.isInternalOnly) {
            var publicPropName = getPropertyName(property, false);
            var privatePropName = getPropertyName(property, true);
            var internalPropertyType = getInternalPropertyType(property);

            if (internalPropertyType === "JsonObject") {
                output += ("\n" + tab + publicPropName + ".stringValue = " + privatePropName + ".StringValue();");
            } else if (internalPropertyType.includes("Array")) {
                output += ("\n" + tab + publicPropName + " = " + privatePropName + ".Empty() ? nullptr : " + privatePropName + ".Data();");
            } else if (internalPropertyType === "String" || internalPropertyType.includes("Vector")) {
                output += ("\n" + tab + publicPropName + " = " + privatePropName + ".empty() ? nullptr : " + privatePropName + ".data();");
            } else if (property.optional) {
                output += ("\n" + tab + publicPropName + " = " + privatePropName + " ? " + privatePropName + ".operator->() : nullptr;");
            } else if (property.isclass) {
                output += ("\n" + tab + publicPropName + " = (" + getPublicPropertyType(property, true) + ")&" + privatePropName + ";");
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

function getFormattedDatatypeDescription(datatype) {
    var output = "/// " + datatype.prefix + datatype.name + " data model.";

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
        // TODO add cref link and full API call name here
        result += call.seeAlso[i].replace(/"/g, "'").replace("/", "") + "Async";
        if (i != call.seeAlso.length - 1) {
            result += ", ";
        }
    }

    return result;
}

function getCallDoc(featureGroupName, call) {
    var asyncName = globalPrefix + featureGroupName + call.name + "Async";
    if (asyncName in xmlRefDocs) {
        return xmlRefDocs[asyncName];
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

function getFormattedCallDescription(featureGroupName, call) {
    return appendToXmlDocComment("///", getCallDoc(featureGroupName, call).summary);
}

function getFormattedCallRemarks(featureGroupName, call) {
    var formattedRemarks = "///";
    
    var callDoc = getCallDoc(featureGroupName, call);
    if (callDoc.remarks !== undefined) {
        formattedRemarks = appendToXmlDocComment(formattedRemarks, callDoc.remarks);
        if (!formattedRemarks.endsWith(".")) {
            formattedRemarks += ".";
        }
        // Leave a blank line before GetResult explanation
        formattedRemarks += ("\n///\n///");
    }

    if (!call.resultDatatype) {
        formattedRemarks = appendToXmlDocComment(formattedRemarks, "Call <see cref=\"XAsyncGetStatus\"/> to get the status of the operation.");
    } else {
        var getResultName = "";
        if (call.resultDatatype.name === "LoginResult") {
            getResultName = globalPrefix + featureGroupName + "ClientLoginGetResult";
        } else if (call.resultDatatype.name === "ServerLoginResult") {
            getResultName = globalPrefix + featureGroupName + "ServerLoginGetResult";
        } else {
            getResultName = globalPrefix + featureGroupName + call.name + "GetResult";
        }
        formattedRemarks = appendToXmlDocComment(formattedRemarks, "If successful, call <see cref=\"" + getResultName + "\"/> to get the result.");
    } 
    return formattedRemarks;
}

function jsonEscapeQuotes(input) {
    if (input !== null) {
        input = input.replace(/"/g, "\\\"");
    }
    return input;
}

function getCorrectedRequestExample(featureGroup, call) {
    var output = JSON.parse(call.requestExample);
    checkReplacements(featureGroup, output);
    return "\"" + jsonEscapeQuotes(jsonEscape(JSON.stringify(output, null, 2))) + "\"";
}

function doReplace(obj, paramName, newValue) {
    if (obj.hasOwnProperty(paramName)) {
        console.log("Replaced: " + obj[paramName] + " with " + newValue);
        obj[paramName] = newValue;
    }
};

function checkReplacements(featureGroup, obj) {
    for (var replaceCategory in propertyReplacements) {
        if (replaceCategory === "generic") {
            for (var genReplaceName1 in propertyReplacements[replaceCategory]) {
                doReplace(obj, genReplaceName1, propertyReplacements[replaceCategory][genReplaceName1]);
            }
        }
        if (replaceCategory === featureGroup.name) {
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

function getRequestExample(featureGroup, call) {
    var msg = null;
    if (call.requestExample.length > 0 && call.requestExample.indexOf("{") >= 0) {
        if (call.requestExample.indexOf("\\\"") === -1) { // I can't handle json in a string in json in a string...
            return getCorrectedRequestExample(featureGroup, call);
        } else {
            msg = "CANNOT PARSE EXAMPLE BODY: ";
        }
    }

    var props = call.requestDatatype.properties;
    var output = {};
    for (var p = 0; p < props.length; p++) {
        output[props[p].name] = props[p].jsontype;
    }

    if (msg === null) {
        msg = "AUTO GENERATED BODY FOR: ";
    }
    console.log(msg + featureGroup.name + "." + call.name);
    // console.log("    " + JSON.stringify(output, null, 2));
    return "\"" + jsonEscapeQuotes(jsonEscape(JSON.stringify(output, null, 2))) + "\"";;
}


