let themeID = 1;

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
        parsed.sort((x, y) => {
            let a = x.name.toUpperCase(), b = y.name.toUpperCase();
                
            return a == b ? 0 : a > b ? 1 : -1;
        });
        parsed.forEach(category => {
            {
                $("#all_features").append(`<div class="text_item title">${category.name}</div>`);

                $("#table_of_content_").append(`<div class="text_item subtitle">${category.name}</div>`);
            }

            const toc_feature = document.createElement("div");
            toc_feature.className = "toc_item"

            category.items.sort((x, y) => {
                let a = x.name.toUpperCase(), b = y.name.toUpperCase();
                
                return a == b ? 0 : a > b ? 1 : -1;
            });
            category.items.forEach(feature => {
                const feature_namespace = feature.namespace;
                const feature_name = feature.name;
                const feature_id = `${feature_namespace}::${feature_name}`;

                const item = `<div class="feature_item">
                    <div class="combine_text" id="${feature_id}">
                        <img class="type_icon" src="assets/${feature.type}.svg" style="color-scheme: ${themeID == 1 ? "light" : "dark"};">
                        <div class="text_item subtitle namespace_text">${feature_namespace}::</div>
                        <div class="text_item subtitle accent_color">${feature_name}</div>
                    </div>
                    <div class="combine_text">
                        <div class="text_item light">available in </div>
                        <div class="text_item subtitle">${feature.infile}</div>
                    </div>
                    <div class="text_item normal description_text">${feature.description}</div>
                </div>`;
                
                $("#all_features").append(item);
                
                $(toc_feature).append(`<div class="text_item">
                                            <a href="#${feature_id}">${feature_name}</a>
                                        </div>`);
            });

            $("#table_of_content_").append(toc_feature);
        });
    });
};
getFeatures();