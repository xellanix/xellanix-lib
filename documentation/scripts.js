let is_dark_theme = false;
let search_filter = "fullname";
let search_key = null;

class FeatureItem {
    constructor(index = 0, catIndex = 0, name = "", namespace = "", filename = "", description = "") {
        this.index = index;
        this.catIndex = catIndex;
        this.name = name.toLowerCase();
        this.namespace = namespace;
        this.filename = filename.toLowerCase();
        this.description = description.toLowerCase();
    }

    get fullname() {
        return `${this.namespace}::${this.name}`;
    }
};

const featureCats = [];
const featureItems = [];

function initiliazeSelectElement() {
    const optionMenus = document.querySelectorAll(".select_menu");
    optionMenus.forEach(optionMenu => {
        const selectButton = optionMenu.querySelector(".select_button"),
                options = optionMenu.querySelectorAll(".option"),
                select_button_text = optionMenu.querySelector(".select_button_text");

        selectButton.addEventListener("click", () => optionMenu.classList.toggle("active"));

        const targetWidth = optionMenu.querySelector(".options").offsetWidth;
        selectButton.style.width = `${targetWidth}px`;
        
        options.forEach(option => {
            option.addEventListener("click", () => {
                // Set the selected option to the select_button
                const selectedOption = option.innerText;
                select_button_text.innerText = selectedOption;
                
                // Hide the option popup after select a option
                optionMenu.classList.remove("active");
            });

            if (option.dataset.optionDefault) {
                // Set the selected option to the select_button
                select_button_text.innerText = option.dataset.optionDefault;
            }
        });

    });
}
initiliazeSelectElement();

function adjustSearchCategory() {
    for (const i in featureCats) {
        const element = featureCats[i];

        if (element.size <= 0) $(".feature_category").eq(i).addClass("hide_for_search");
        else $(".feature_category").eq(i).removeClass("hide_for_search");
    }
}

function searchFeatureByFilter(filterBase = "", index = 0, catIndex = 0) {
    if (filterBase.indexOf(search_key) != -1) {
        // Show the feature
        $("#all_features").children().eq(index).removeClass("hide_for_search");
        featureCats[catIndex].add(index);
    }
    else {
        // Hide it
        $("#all_features").children().eq(index).addClass("hide_for_search");
        featureCats[catIndex].delete(index);
    }
}

function searchFeature() {
    for (const item of featureItems) {
        switch (search_filter) {
            case "name":
                searchFeatureByFilter(item.name, item.index, item.catIndex);
                break;
            case "namespace":
                searchFeatureByFilter(item.namespace, item.index, item.catIndex);
                break;
            case "fullname":
                searchFeatureByFilter(item.fullname, item.index, item.catIndex);
                break;
            case "file":
                searchFeatureByFilter(item.filename, item.index, item.catIndex);
                break;
            case "description":
                searchFeatureByFilter(item.description, item.index, item.catIndex);
                break;
            default:
                searchFeatureByFilter(item.name, item.index, item.catIndex);
                break;
        }
    }

    adjustSearchCategory();
}

$("#search_filter .select_button").click(function() {
    $("#search_section").toggleClass("stay_focus");
})

$("#search_filter .option").click(function() {
    search_filter = $(this).text().substring(3);
    $("#search_section").removeClass("stay_focus");
    
    if (isAvailable(search_key)) searchFeature();
});

$("#search_box").on("input", function() {
    search_key = $(this).val().toLowerCase();

    searchFeature();
});

function isAvailable(value = "") {
    if (typeof value === "string" && value.length === 0) return false;
    else if (value === null) return false;
    else return true;
}

function setURLParams(name, value, is_remove = false) {
    const url = new URL(window.location.href);
    const searchParams = url.searchParams;

    if (!is_remove) searchParams.set(name, value);
    else searchParams.delete(name);

    // window.location.href = url.toString();
    window.history.pushState({}, "", url.toString());
};

$(".theme_option_radio").change(function() {
    const _this = $(this);
    if (_this.is(':checked')) {
        const theme_id = _this.val();
        $("#root").prop("class", theme_id);
        is_dark_theme = theme_id == "darktheme";
        setURLParams("theme", theme_id, !is_dark_theme);
    };
});

$(document).ready(function(e) {
    const url = new URL(window.location.href);
    const searchParams = url.searchParams;

    const currentTheme = searchParams.get("theme");
    if (currentTheme == "darktheme") {
        $("#root").prop("class", currentTheme);
        $("#dark_theme").prop("checked", true);
    }
});

function goToLastHash() {
    const hash = window.location.hash.substring(1);
    const targetElement = document.getElementById(hash);

    if (targetElement) {
        const targetPosition = targetElement.offsetTop;
        $("#all_features").scrollTop(targetPosition);
    }
}

function sortByName(x, y) {
    const a = x.name.toUpperCase(), b = y.name.toUpperCase();

    return a == b ? 0 : a > b ? 1 : -1;
}

function setSyntaxColor(syntax = "", definedTypes = [""]) {
    const keywords = ["const", "constexpr", "typename", "decltype", "using"];
    const primitives = ["int", "double", "bool", "auto"];

    let colorize = syntax;
    for (const keyword of keywords) {
        colorize = colorize.replace(new RegExp(`\\b${keyword}\\b`, "g"), '<span class="keyword">$&</span>');
    }
    for (const primitive of primitives) {
        colorize = colorize.replace(new RegExp(`\\b${primitive}\\b`, "g"), '<span class="primitive">$&</span>');
    }
    for (const definedType of definedTypes) {
        if (definedType == "") continue;

        colorize = colorize.replace(new RegExp(`\\b${definedType}\\b`, "g"), '<span class="definedType">$&</span>');
    }

    return colorize;
}

function getFeatures() {
    /*
    <div class="text_item title">Arithmetics</div>
    <div class="feature_item">
        <div class="combine_text">
            <img class="type_icon" src="assets/object.svg"/>
            <div class="text_item subtitle namespace_text">xellanix::type::</div>
            <div class="text_item subtitle accent_color">number</div>
        </div>
        <div class="combine_text">
            <div class="text_item light">available in </div>
            <div class="text_item subtitle">number.h</div>
        </div>
        <div class="text_item normal description_text">Lorem ipsum dolor sit amet, consectetur adipiscing elit.</div>
    </div>
    */
    fetch('./features.json').then((response) => response.json()).then(function(json) {
        let parsed = JSON.parse(JSON.stringify(json));
        $("#xlib-version").text(`version ${parsed.version}`);

        parsed.xlib.sort(sortByName).forEach(category => {
            $("#all_features").append(`<div class="text_item title feature_category">${category.name}</div>`);
            {
                const toc = $(`<div class="text_item subtitle expander">${category.name}</div>`).appendTo("#table_of_content_");
                
                toc.click(function() {
                    $(this).toggleClass("isExpanded");
                    let content = $(this).next();
                    
                    if (content.css("max-height") !== "0px") {
                        content.css({
                            "min-height": 0,
                            "max-height": 0
                        });
                    } else {
                        content.css({
                            "min-height": content.prop("scrollHeight") + "px",
                            "max-height": content.prop("scrollHeight") + "px"
                        });
                    }
                });
            }

            const toc_feature = document.createElement("div");
            toc_feature.className = "toc_item";

            const featureIndexes = new Set();
            category.items.sort(sortByName).forEach(feature => {
                const feature_namespace = feature.namespace;
                const feature_name = feature.name;
                const feature_id = `${feature_namespace}::${feature_name}`;

                let params = "";
                if (feature.anyParams) {
                    params += '<div class="text_item subtitle" style="margin-top: 6px">Syntax</div>';
                    params += '<div class="syntax-container">';
                    feature.anyParams.forEach(param => {
                        /*
                        < = &lt;
                        > = &gt;
                        " = &quot;
                        & = &amp;
                        */
                        
                        const syntax = `${param.returnType} ${feature_name}${isAvailable(param.templateParam) ? `&lt;${param.templateParam}&gt;` : ""}(${param.functionParam})`;
                        params += `<div>
                            <p class="text_item">${setSyntaxColor(syntax, param.definedType)}</p>
                            <div class="syntax-description-root">
                                ${isAvailable(param.templateParam) ? `<div class="syntax-description">
                                    <p class="description-type">Template Parameter:</p>
                                    <p class="description-value">${param.templateParamDescription}</p>
                                </div>` : ""}
                                ${isAvailable(param.functionParam) ? `<div class="syntax-description">
                                    <p class="description-type">Parameter:</p>
                                    <p class="description-value">${param.functionParamDescription}</p>
                                </div>` : ""}
                                ${isAvailable(param.returnType) ? `<div class="syntax-description">
                                    <p class="description-type">Return Value:</p>
                                    <p class="description-value">${param.returnDescription}</p>
                                </div>` : ""}
                            </div>
                        </div>`;
                    });
                    params += '</div>';
                }

                const item = `<section class="feature_item">
                    <div class="combine_text" id="${feature_id}">
                        <img class="type_icon" src="assets/${feature.type}.svg">
                        <div class="text_item subtitle feature_compl_name">
                            <div>${feature_namespace}::</div>
                            <div class="accent_color">${feature_name}</div>
                        </div>
                    </div>
                    <div class="combine_text">
                        <div class="text_item light">available in </div>
                        <div class="text_item subtitle">${feature.infile}</div>
                    </div>
                    <p class="text_item normal description_text">${feature.description}</p>
                    ${params}
                </section>`;

                const currentIndex = $("#all_features").children().length;
                featureItems.push(new FeatureItem(currentIndex,
                                                    featureCats.length, 
                                                    feature_name, 
                                                    feature_namespace, 
                                                    feature.infile, 
                                                    feature.description));
                
                $("#all_features").append(item);
                
                $(toc_feature).append(`<div class="text_item">
                                            <a href="#${feature_id}">${feature_name}</a>
                                        </div>`);

                featureIndexes.add(currentIndex);
            });

            featureCats.push(featureIndexes);
            $("#table_of_content_").append(toc_feature);
        });

        goToLastHash();
    });
};
getFeatures();