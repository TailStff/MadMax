declare var DevExpress: any;
declare var $: any;

$(function () {

    type Accum = {

        name: string;
        value: any;
        type: number;
    };

    type AccumDetail = {

        name: string;
        value: any;
        type: number;
    };

    type Accums = Accum[];

    function isDev(): boolean {
        return window.location.protocol === "file:";
    }

    // the widget definition, where "custom" is the namespace,
    // "AccumsNavigator" the widget name
    $.widget("custom.AccumsNavigator", {

        container: null,
        manageAccumsContainer: null,
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

            let self = this;
            
            self.interval && clearInterval(self.interval);
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

            self.manageAccumsContainer = $("<div id='AccumsContainer'></div>").appendTo(self.container);

            let list = $("<ul></ul>").appendTo(self.manageAccumsContainer);
            let detail = $("<div id='AccumDetail'></div>").appendTo(self.manageAccumsContainer);

            self.getList()
                .then((json: Accums | null) => {

                    if (json === null) {

                        console.warn("getList is busy, please wait.");
                        return;
                    }

                    json.forEach((Accum: Accum) => {

                        let item = $("<li></li>").appendTo(list);
                        item.append($("<span class='icon'></span>"));
                        item.append($("<span></span>").text(Accum.name));

                        item.on("click", () => {

                            self.interval && clearInterval(self.interval);                  // Clear previous interval if it exists

                            item.addClass("selected").siblings().removeClass("selected");   // Set selected class on the clicked item and remove it from siblings
                            self.uiPrepareAccumDetail(Accum, detail);                       // Prepare the detail container for new content
                            self.uiDisplayAccumValueEditor(Accum, detail);                  // Display value editor interface immediately, then start polling for updates

                            self.interval = setInterval(() => {

                                self.getDetails(Accum.name)
                                    .then((json: AccumDetail | null) => {

                                        if (json === null)
                                            return;

                                        // Update memorized value with current server value
                                        Accum.value = json.value;

                                        self.uiDisplayAccumDetail(json, detail);
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

        /// Get Accums list from the server, returns null if a request is already in progress, otherwise returns a promise that resolves to the Accums list
        getList: async function (): Promise<Accums | null> {

            if (this.getData_busy)
                return null;

            this.getData_busy = true;

            try {

                // Simulate API response with mock data in development, otherwise fetch from the server
                if (isDev()) {

                    const mock: Accums = [

                        { name: "test_bool", value: false, type: 11 },
                        { name: "test_int", value: 42, type: 7 },
                        { name: "test_float", value: 3.14, type: 2 },
                        { name: "test_double", value: 3.1415, type: 1 }
                    ];

                    // petit délai pour simuler un vrai fetch (optionnel)
                    await new Promise(r => setTimeout(r, 100));

                    return mock;
                } else {

                    const response = await fetch("/API/Accums");

                    if (!response.ok) {
                        throw new Error(`HTTP ${response.status}`);
                    }

                    const json = await response.json();

                    return json as Accums;
                }

            } finally {
                this.getData_busy = false;
            }
        },

        /// Get Accums list from the server, returns null if a request is already in progress, otherwise returns a promise that resolves to the Accums list
        getDetails: async function (name: string): Promise<AccumDetail | null> {

            if (this.getData_busy)
                return null;

            this.getData_busy = true;

            try {

                // Simulate API response with mock data in development, otherwise fetch from the server
                if (isDev()) {

                    let mock: AccumDetail = { name: name, value: null, type: 0 };

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

                    const response = await fetch(`/API/Accums/${name}`);

                    if (!response.ok) {
                        throw new Error(`HTTP ${response.status}`);
                    }

                    const json = await response.json();

                    return json as AccumDetail;
                }

            } finally {
                this.getData_busy = false;
            }
        },

        uiPrepareAccumDetail: function (Accum: Accum, detail: any) {

            let AccumDataType: String = this.getAccumDataType(Accum.type);

            detail.html(''); // Clear previous content
            detail.append(`<div class="title">Propriété de l'Accum</div>`);
            detail.append(`<div class="property name"><span>Désignation :</span><span id="Accum-name">${Accum.name}</span></div>`);
            detail.append(`<div class="property value"><span>Valeur :</span><span id="Accum-value" class='stale'>-</span></div>`);
            detail.append(`<div class="property type"><span>Type :</span><span id="Accum-type">${AccumDataType}</span></div>`);
        },

        uiDisplayAccumValueEditor: function (Accum: Accum, container: any) {

            let self = this;

            container.append(`<div class="title">Édition des propriétés</div>`);
            let form = $("<form></form>").appendTo(container);
            let div = $("<div class='property name'></div>").appendTo(form).append(`<span>Valeur :</span>`);

            switch (Accum.type) {

                case 1: // double
                    div.append(`<input id='value' type='number' value='${Accum.value}' step='any'/>`);
                    break;
                case 2: // float
                    div.append(`<input id='value' type='number' value='${Accum.value}' step='any'/>`);
                    break;
                case 3: // int64
                    div.append(`<input id='value' type='number' value='${Accum.value}' min='-9223372036854775808' max='9223372036854775807' step='1'/>`);
                    break;
                case 4: // uint64
                    div.append(`<input id='value' type='number' value='${Accum.value}' min='0' max='18446744073709551615' step='1'/>`);
                    break;
                case 5: // int32
                    div.append(`<input id='value' type='number' value='${Accum.value}' min='-2147483648' max='2147483647' step='1'/>`);
                    break;
                case 6: // uint32
                    div.append(`<input id='value' type='number' value='${Accum.value}' min='0' max='4294967295' step='1'/>`);
                    break;
                case 7: // int16
                    div.append(`<input id='value' type='number' value='${Accum.value}' min='-32768' max='32767' step='1'/>`);
                    break;
                case 8: // uint16
                    div.append(`<input id='value' type='number' value='${Accum.value}' min='0' max='65535' step='1'/>`);
                    break;
                case 9: // int8
                    div.append(`<input id='value' type='number' value='${Accum.value}' min='-128' max='127' step='1'/>`);
                    break;
                case 10: // uint8
                    div.append(`<input id='value' type='number' value='${Accum.value}' min='0' max='255' step='1'/>`);
                    break;
                case 11: // bool
                    div.append(`<input id='value' type='checkbox' ${Accum.value === true ? "checked='checked'" : ""} value='true'/>`);
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

                switch (Accum.type) {

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

                console.log("Updating Accum with new value:", JSON.stringify(newValue));

                self.setAccumValue(Accum.name, newValue)
                    .then(() => {
                        $(".property #Accum-value", container).html(newDisplayedValue).addClass("stale"); // Update displayed value and add stale class as it may take a moment for the new value to be reflected in the details view
                        submitButton.removeAttr("disabled");
                    })
                    .catch((error: string) => {
                        console.error("Error updating Accum:", error);
                        submitButton.removeAttr("disabled");
                    });

                return false;
            });
        },

        getAccumDataType: function (type: number): string {

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

        uiDisplayAccumDetail: function (Accum: AccumDetail, container: any) {

            if (container.find("#Accum-name").html() == Accum.name) {

                container.find("#Accum-value").removeClass("stale"); // Remove stale class to indicate value is up-to-date

                switch (Accum.type) {

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
                        container.find("#Accum-value").html(Accum.value);
                        break;
                    case 11: // bool
                        container.find("#Accum-value").html(Accum.value ? "true" : "false");
                        break;
                }
            }
        },

        setAccumValue: async function (AccumName: string, newValue: any): Promise<void> {

            try {
                const response = await fetch(`/API/Accums/${AccumName}`, {
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
                console.error("Error in setAccumValue: ", error);
                throw error;
            }
        }
    });
});
