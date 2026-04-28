declare var DevExpress: any;
declare var $: any;

$(function () {

    type Variable = {
        
        name: string;
        value: any;
        type: number;
    };

    type VariableDetail = {

        name: string;
        value: any;
        type: number;
    };

    type Variables = Variable[];

    function isDev(): boolean {
        return window.location.protocol === "file:";
    }

    // the widget definition, where "custom" is the namespace,
    // "VariableNavigator" the widget name
    $.widget("custom.VariableNavigator", {

        container: null,
        manageVariablesContainer: null,
        getData_busy: false,
        interval: null,

        options: {},

        // The constructor, set DOM
        _create: function () {

            let self = this;

            self.container = self.element;

            self._uiCreateComposant();

            return self;
        },

        // Refresh composant (js -> css styling, resize (less possible),…)
        _refresh: function () {

        },

        // Events bound via _on are removed automatically
        // revert other modifications here, remove DOM
        _destroy: function () {

        },

        // _setOptions is called with a hash of all options that are changing
        // always refresh when changing options
        _setOptions: function () {

            // _super and _superApply handle keeping the right this-context
            this._superApply(arguments);
            this._refresh();
        },

        // _setOption is called for each individual option that is changing
        _setOption: function (key: any, value: any) {

            // we can disable change in return before _super

            this._super(key, value);
        },

        _uiCreateComposant: function () {

            let self = this;

            self.container.html('');

            self.manageVariablesContainer = $("<div id='VariablesContainer'></div>").appendTo(self.container);

            let list = $("<ul></ul>").appendTo(self.manageVariablesContainer);
            let detail = $("<div id='VariableDetail'></div>").appendTo(self.manageVariablesContainer);

            self.getList()
                .then((json: Variables | null) => {

                    if (json === null) {

                        console.warn("getList is busy, please wait.");
                        return;
                    }

                    json.forEach((variable: Variable) => {

                        let item = $("<li></li>").appendTo(list);
                        item.append($("<span class='icon'></span>"));
                        item.append($("<span></span>").text(variable.name));

                        item.on("click", () => {

                            // Set selected class on the clicked item and remove it from siblings
                            item.addClass("selected").siblings().removeClass("selected");

                            // Prepare the detail container for new content
                            self.uiPrepareVariableDetail(detail);
                            self.uiDisplaySetVariableValue(variable, detail);

                            self.interval && clearInterval(self.interval); // Clear previous interval if it exists
                            self.interval = setInterval(() => {

                                self.getDetails(variable.name)
                                    .then((json: VariableDetail | null) => {

                                        if (json === null) {

                                            console.warn("getDetails is busy, please wait.");
                                            return;
                                        }

                                        self.uiDisplayVariableDetail(json, detail);
                                    })
                                    .catch((error: any) => {
                                        console.error("Erreur getDetails:", error);
                                    });
                            }, 1000);
                        });

                    });

                })
                .catch((error: any) => {

                    console.error("Erreur getData:", error);
                });
        },

        /// Get variables list from the server, returns null if a request is already in progress, otherwise returns a promise that resolves to the variables list
        getList: async function (): Promise<Variables | null> {

            if (this.getData_busy)
                return null;

            this.getData_busy = true;

            try {

                // Simulate API response with mock data in development, otherwise fetch from the server
                if (isDev()) {

                    const mock: Variables = [

                        { name: "test_bool", value: false, type: 11 },
                        { name: "test_int", value: 42, type: 7 },
                        { name: "test_float", value: 3.14, type: 2 },
                        { name: "test_double", value: 3.1415, type: 1 }
                    ];

                    // petit délai pour simuler un vrai fetch (optionnel)
                    await new Promise(r => setTimeout(r, 100));

                    return mock;
                } else {

                    const response = await fetch("/API/Variables");

                    if (!response.ok) {
                        throw new Error(`HTTP ${response.status}`);
                    }

                    const json = await response.json();

                    return json as Variables;
                }

            } finally {
                this.getData_busy = false;
            }
        },

        /// Get variables list from the server, returns null if a request is already in progress, otherwise returns a promise that resolves to the variables list
        getDetails: async function (name: string): Promise<VariableDetail | null> {

            if (this.getData_busy)
                return null;

            this.getData_busy = true;

            try {

                // Simulate API response with mock data in development, otherwise fetch from the server
                if (isDev()) {

                    let mock: VariableDetail = { name: name, value: null, type: 0 };

                    switch (name) {

                        case "test_bool":
                            mock.value = false;
                            mock.type = 11;
                            mock.name = "test_bool";
                            break;
                        case "test_int":
                            mock.value = 42;
                            mock.type = 7;
                            mock.name = "test_int";
                            break;
                        case "test_float":
                            mock.value = 3.14;
                            mock.type = 2;
                            mock.name = "test_float";
                            break;
                        case "test_double":
                            mock.value = 3.1415;
                            mock.type = 1;
                            mock.name = "test_double";
                            break;
                    };

                    // petit délai pour simuler un vrai fetch (optionnel)
                    await new Promise(r => setTimeout(r, 100));

                    return mock;
                } else {

                    const response = await fetch(`/API/Variables/${name}`);

                    if (!response.ok) {
                        throw new Error(`HTTP ${response.status}`);
                    }

                    const json = await response.json();

                    return json as VariableDetail;
                }

            } finally {
                this.getData_busy = false;
            }
        },

        uiPrepareVariableDetail: function (detail: any) {

            detail.html(''); // Clear previous content
            detail.append(`<div class="title">Propriété de la variable</div>`);
            detail.append(`<div class="property name"><span>Désignation :</span><span id="variable-name">-</span></div>`);
            detail.append(`<div class="property value"><span>Valeur :</span><span id="variable-value">-</span></div>`);
            detail.append(`<div class="property type"><span>Type :</span><span id="variable-type">-</span></div>`);
        },

        uiDisplaySetVariableValue: function (variable: Variable, container: any) {

            let self = this;

            container.append(`<div class="title">Mise à jour de la valeur</div>`);
            let form = $("<form></form>").appendTo(container);

            switch (variable.type) {

                case 1: // double
                    form.append(`<div class="property value"><span>Nouvelle valeur :</span><input id='value' type='number' value='${variable.value}' step='any' /></div>`);
                    break;
                case 2: // float
                    form.append(`<div class="property value"><span>Nouvelle valeur :</span><input id='value' type='number' value='${variable.value}' step='any' /></div>`);
                    break;
                case 3: // int64
                    form.append(`<div class="property value"><span>Nouvelle valeur :</span><input id='value' type='number' value='${variable.value}' min='-9223372036854775808' max='9223372036854775807' step='1' /></div>`);
                    break;
                case 4: // uint64
                    form.append(`<div class="property value"><span>Nouvelle valeur :</span><input id='value' type='number' value='${variable.value}' min='0' max='18446744073709551615' step='1' /></div>`);
                    break;
                case 5: // int32
                    form.append(`<div class="property value"><span>Nouvelle valeur :</span><input id='value' type='number' value='${variable.value}' min='-2147483648' max='2147483647' step='1' /></div>`);
                    break;
                case 6: // uint32
                    form.append(`<div class="property value"><span>Nouvelle valeur :</span><input id='value' type='number' value='${variable.value}' min='0' max='4294967295' step='1' /></div>`);
                    break;
                case 7: // int16
                    form.append(`<div class="property value"><span>Nouvelle valeur :</span><input id='value' type='number' value='${variable.value}' min='-32768' max='32767' step='1' /></div>`);
                    break;
                case 8: // uint16
                    form.append(`<div class="property value"><span>Nouvelle valeur :</span><input id='value' type='number' value='${variable.value}' min='0' max='65535' step='1' /></div>`);
                    break;
                case 9: // int8
                    form.append(`<div class="property value"><span>Nouvelle valeur :</span><input id='value' type='number' value='${variable.value}' min='-128' max='127' step='1' /></div>`);
                    break;
                case 10: // uint8
                    form.append(`<div class="property value"><span>Nouvelle valeur :</span><input id='value' type='number' value='${variable.value}' min='0' max='255' step='1' /></div>`);
                    break;
                case 11: // bool
                    form.append(`<div class="property value"><span>Nouvelle valeur :</span><input id='value' type='checkbox' ${variable.value === true ? "checked:'checked'":""} value='true' /></div>`);
                    break;

            }
            // Add submit button
            $(`<div class="button"><button type='submit'>Mettre à jour</button></div>`).appendTo(form);

            // Handle form submission
            form.on("submit", (e: any) => {

                e.preventDefault();
                let newValue: any = { value: null };
                switch (variable.type) {

                    case 1: // double
                    case 2: // float
                        newValue.value = parseFloat(form.find("#value").val());
                        break;

                    case 3: // int64
                    case 4: // uint64
                    case 5: // int32
                    case 6: // uint32
                    case 7: // int16
                    case 8: // uint16
                    case 9: // int8
                    case 10: // uint8
                        newValue.value = parseInt(form.find("#value").val());
                        break;

                    case 11: // bool
                        newValue.value = form.find("#value").is(":checked");
                        break;
                }
                self.setVariableValue(variable.name, newValue)
                    .then(() => {
                        console.log("Variable updated successfully");
                    })
                    .catch((error: string) => {
                        console.error("Error updating variable:", error);
                    });

                return false;
            });
        },

        uiDisplayVariableDetail: function (variable: VariableDetail, container: any) {

            // Ici, vous pouvez personnaliser l'affichage en fonction du type de variable
            // Par exemple, pour les booléens, vous pourriez afficher une icône ou un interrupteur
            // Pour les nombres, vous pourriez formater l'affichage différemment

            container.find("#variable-name").html(variable.name);

            switch (variable.type) {

                case 1: // double
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("64 bit double");
                    break;
                case 2: // float
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("32 bit float");
                    break;
                case 3: // int64
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("64 bit signed integer");
                    break;
                case 4: // uint64
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("64 bit unsigned integer");
                    break;
                case 5: // int32
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("32 bit signed integer");
                    break;
                case 6: // uint32
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("32 bit unsigned integer");
                    break;
                case 7: // int16
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("16 bit signed integer");
                    break;
                case 8: // uint16
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("16 bit unsigned integer");
                    break;
                case 9: // int8
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("8 bit signed integer");
                    break;
                case 10: // uint8
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("8 bit unsigned integer");
                    break;
                case 11: // bool
                    container.find("#variable-value").html(variable.value ? "True" : "False");
                    container.find("#variable-type").html("Boolean");
                    break;
            }
        },

        setVariableValue: async function (variableName: string, newValue: any): Promise<void> {

            try {
                const response = await fetch(`/API/Variables/${variableName}`, {
                    method: "POST",
                    headers: {
                        "Content-Type": "application/json"
                    },
                    body: JSON.stringify(newValue)
                });
                if (!response.ok) {
                    throw new Error(`HTTP ${response.status}`);
                }
            } catch (error) {
                console.error("Error in setVariableValue: ", error);
                throw error;
            }
        }
    });
});
