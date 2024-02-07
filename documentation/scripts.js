class a {

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
        parsed.forEach(element => {
            {
                const feature = document.createElement("div");
                feature.className = "text_item title";
                feature.innerText = element.name;

                document.getElementById("all_features").appendChild(feature);

                const toc = feature.cloneNode(true);
                toc.className = "text_item subtitle";
                document.getElementById("table_of_content_").appendChild(toc);
            }

            const toc_subfeature = document.createElement("div");
            toc_subfeature.className = "toc_item"

            element.items.forEach(subfeature => {
                const item = document.createElement("div");
                item.className = "feature_item";
                {
                    const subfeature_namespace = subfeature.namespace;
                    const subfeature_name = subfeature.name;
                    const subfeature_id = `${subfeature_namespace}::${subfeature_name}`;

                    const fullname = document.createElement("div");
                    fullname.className = "combine_text";
                    fullname.id = subfeature_id;
                    {
                        const type_icon = document.createElement("img");
                        type_icon.className = "type_icon";
                        type_icon.src = `assets/${subfeature.type}.svg`;
                        fullname.appendChild(type_icon);
                    }
                    {
                        const namespace_text = document.createElement("div");
                        namespace_text.className = "text_item subtitle namespace_text";
                        namespace_text.innerText = `${subfeature_namespace}::`;
                        fullname.appendChild(namespace_text);
                    }
                    {
                        const feature_name = document.createElement("div");
                        feature_name.className = "text_item subtitle accent_color";
                        feature_name.innerText = subfeature_name;

                        fullname.appendChild(feature_name);
                    }
                    {
                        const toc_subfeature_item = document.createElement("div");
                        toc_subfeature_item.className = "text_item";
                        {
                            const toc_subfeature_link = document.createElement("a");
                            toc_subfeature_link.href = `#${subfeature_id}`;
                            toc_subfeature_link.innerText = subfeature_name;
                            toc_subfeature_item.appendChild(toc_subfeature_link);
                        }

                        toc_subfeature.appendChild(toc_subfeature_item);
                    }
                    
                    item.appendChild(fullname);
                }
                {
                    const infile = document.createElement("div");
                    infile.className = "combine_text";
                    {
                        const available_in = document.createElement("div");
                        available_in.className = "text_item light";
                        available_in.innerText = "available in ";
                        infile.appendChild(available_in);
                    }
                    {
                        const filaname = document.createElement("div");
                        filaname.className = "text_item subtitle";
                        filaname.innerText = subfeature.infile;
                        infile.appendChild(filaname);
                    }

                    item.appendChild(infile);
                }

                const description = document.createElement("div");
                description.className = "text_item normal description_text";
                description.innerText = subfeature.description;
                item.appendChild(description);
                
                document.getElementById("all_features").appendChild(item);
            });

            document.getElementById("table_of_content_").appendChild(toc_subfeature);
        });
    });
};
getFeatures();