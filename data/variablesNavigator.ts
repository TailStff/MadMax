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

                            self.interval && clearInterval(self.interval);                  // Clear previous interval if it exists

                            item.addClass("selected").siblings().removeClass("selected");   // Set selected class on the clicked item and remove it from siblings
                            self.uiPrepareVariableDetail(variable, detail);                 // Prepare the detail container for new content
                            self.uiDisplayVariableValueEditor(variable, detail);            // Display value editor interface immediately, then start polling for updates

                            self.interval = setInterval(() => {

                                self.getDetails(variable.name)
                                    .then((json: VariableDetail | null) => {

                                        if (json === null)
                                            return;

                                        // Update memorized value with current server value
                                        variable.value = json.value;

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

        uiPrepareVariableDetail: function (variable: Variable, detail: any) {

            let variableDataType: String = this.getVariableDataType(variable.type);

            detail.html(''); // Clear previous content
            detail.append(`<div class="title">Propriété de la variable</div>`);
            detail.append(`<div class="property name"><span>Désignation :</span><span id="variable-name">${variable.name}</span></div>`);
            detail.append(`<div class="property value"><span>Valeur :</span><span id="variable-value" class='stale'>-</span></div>`);
            detail.append(`<div class="property type"><span>Type :</span><span id="variable-type">${variableDataType}</span></div>`);
        },

        uiDisplayVariableValueEditor: function (variable: Variable, container: any) {

            let self = this;

            container.append(`<div class="title">Édition des propriétés</div>`);
            let form = $("<form></form>").appendTo(container);
            let div = $("<div class='property name'></div>").appendTo(form).append(`<span>Valeur :</span>`);

            switch (variable.type) {

                case 1: // double
                    div.append(`<input id='value' type='number' value='${variable.value}' step='any'/>`);
                    break;
                case 2: // float
                    div.append(`<input id='value' type='number' value='${variable.value}' step='any'/>`);
                    break;
                case 3: // int64
                    div.append(`<input id='value' type='number' value='${variable.value}' min='-9223372036854775808' max='9223372036854775807' step='1'/>`);
                    break;
                case 4: // uint64
                    div.append(`<input id='value' type='number' value='${variable.value}' min='0' max='18446744073709551615' step='1'/>`);
                    break;
                case 5: // int32
                    div.append(`<input id='value' type='number' value='${variable.value}' min='-2147483648' max='2147483647' step='1'/>`);
                    break;
                case 6: // uint32
                    div.append(`<input id='value' type='number' value='${variable.value}' min='0' max='4294967295' step='1'/>`);
                    break;
                case 7: // int16
                    div.append(`<input id='value' type='number' value='${variable.value}' min='-32768' max='32767' step='1'/>`);
                    break;
                case 8: // uint16
                    div.append(`<input id='value' type='number' value='${variable.value}' min='0' max='65535' step='1'/>`);
                    break;
                case 9: // int8
                    div.append(`<input id='value' type='number' value='${variable.value}' min='-128' max='127' step='1'/>`);
                    break;
                case 10: // uint8
                    div.append(`<input id='value' type='number' value='${variable.value}' min='0' max='255' step='1'/>`);
                    break;
                case 11: // bool
                    div.append(`<input id='value' type='checkbox' ${variable.value === true ? "checked='checked'" : ""} value='true'/>`);
                    break;
            }

            // Add submit button
            let buttons = $("<div class='buttons'>").appendTo(form);
            let submitButton = $("<button type='submit'>Mettre à jour</button></div>").appendTo(buttons);

            // Handle form submission
            form.on("submit", (e: any) => {

                e.preventDefault();

                submitButton.attr("disabled", "disabled");

                let newValue: any = { value: null };
                let newDisplayedValue: any = null;

                switch (variable.type) {

                    case 1: // double
                    case 2: // float
                        newValue.value = parseFloat(form.find("#value").val());
                        newDisplayedValue = newValue.value;
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
                        newDisplayedValue = newValue.value;
                        break;

                    case 11: // bool
                        newValue.value = form.find("#value").is(":checked");
                        newDisplayedValue = newValue.value ? "true" : "false";
                        break;
                }

                console.log("Updating variable with new value:", JSON.stringify(newValue));

                self.setVariableValue(variable.name, newValue)
                    .then(() => {
                        $(".property #variable-value", container).html(newDisplayedValue).addClass("stale"); // Update displayed value and add stale class as it may take a moment for the new value to be reflected in the details view
                        submitButton.removeAttr("disabled");
                    })
                    .catch((error: string) => {
                        console.error("Error updating variable:", error);
                        submitButton.removeAttr("disabled");
                    });

                return false;
            });
        },

        getVariableDataType: function (type: number): string {

            switch (type) {

                case 1: return "64 bit double";
                case 2: return "32 bit float";
                case 3: return "64 bit signed integer";
                case 4: return "64 bit unsigned integer";
                case 5: return "32 bit signed integer";
                case 6: return "32 bit unsigned integer";
                case 7: return "16 bit signed integer";
                case 8: return "16 bit unsigned integer";
                case 9: return "8 bit signed integer";
                case 10: return "8 bit unsigned integer";
                case 11: return "Boolean";
            }

            return "Unknown";
        },

        uiDisplayVariableDetail: function (variable: VariableDetail, container: any) {

            if (container.find("#variable-name").html() == variable.name) {

                container.find("#variable-value").removeClass("stale"); // Remove stale class to indicate value is up-to-date

                switch (variable.type) {

                    case 1: // double
                    case 2: // float
                    case 3: // int64
                    case 4: // uint64
                    case 5: // int32
                    case 6: // uint32
                    case 7: // int16
                    case 8: // uint16
                    case 9: // int8
                    case 10: // uint8
                        container.find("#variable-value").html(variable.value);
                        break;
                    case 11: // bool
                        container.find("#variable-value").html(variable.value ? "true" : "false");
                        break;
                }
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
