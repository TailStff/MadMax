declare var DevExpress: any;
declare var $: any;

$(function () {

    type PumpSwap = {

        name: string;
        values: boolean[];
        actions: string[];
    };

    type PumpState = {

        command: boolean;
        feedback: boolean;
        value: boolean;
        runTime: number;
        startCount: number;
        fault: boolean;
    };

    type PumpSwapDetail = {

        name: string;
        values: boolean[];
        actions: string[];
        availablePumps: number;
        capacityState: number;
        requestedPumps: number;
        runningPumps: number;
        totalPumps: number;
        [key: number]: PumpState;
    };

    type PumpSwaps = PumpSwap[];

    function isDev(): boolean {
        return window.location.protocol === "file:";
    }

    // the widget definition, where "custom" is the namespace,
    // "PumpSwapsNavigator" the widget name
    $.widget("custom.PumpSwapsNavigator", {

        container: null,
        managePumpSwapsContainer: null,
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

            self.managePumpSwapsContainer = $("<div id='PumpSwapsContainer'></div>").appendTo(self.container);

            let list = $("<ul></ul>").appendTo(self.managePumpSwapsContainer);
            let detail = $("<div id='PumpSwapDetail'></div>").appendTo(self.managePumpSwapsContainer);

            self.getList()
                .then((json: PumpSwaps | null) => {

                    if (json === null) {

                        console.warn("getList is busy, please wait.");
                        return;
                    }

                    json.forEach((pumpSwap: PumpSwap) => {

                        let item = $("<li></li>").appendTo(list);
                        item.append($("<span class='icon'></span>"));
                        item.append($("<span></span>").text(pumpSwap.name));

                        item.on("click", () => {

                            self.interval && clearInterval(self.interval);                  // Clear previous interval if it exists

                            item.addClass("selected").siblings().removeClass("selected");   // Set selected class on the clicked item and remove it from siblings
                            self.uiPreparePumpSwapDetail(pumpSwap, detail);                 // Prepare the detail container for new content
                            self.uiDisplayPumpSwapValueEditor(pumpSwap, detail);            // Display value editor interface immediately, then start polling for updates

                            self.interval = setInterval(() => {

                                self.getDetails(pumpSwap.name)
                                    .then((json: PumpSwapDetail | null) => {

                                        if (json === null)
                                            return;

                                        // Update memorized value with current server value
                                        pumpSwap.values = json.values;

                                        self.uiDisplayPumpSwapDetail(json, detail);
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

        /// Get pumpSwaps list from the server, returns null if a request is already in progress, otherwise returns a promise that resolves to the pumpSwaps list
        getList: async function (): Promise<PumpSwaps | null> {

            if (this.getData_busy)
                return null;

            this.getData_busy = true;

            try {

                // Simulate API response with mock data in development, otherwise fetch from the server
                if (isDev()) {

                    const mock: PumpSwaps = [

                        { name: "test_4pump", values: [false, false, false, false], actions: ["Reset"] }
                    ];

                    // petit délai pour simuler un vrai fetch (optionnel)
                    await new Promise(r => setTimeout(r, 100));

                    return mock;
                } else {

                    const response = await fetch("/API/PumpSwaps");

                    if (!response.ok) {
                        throw new Error(`HTTP ${response.status}`);
                    }

                    const json = await response.json();

                    return json as PumpSwaps;
                }

            } finally {
                this.getData_busy = false;
            }
        },

        /// Get pumpSwaps list from the server, returns null if a request is already in progress, otherwise returns a promise that resolves to the pumpSwaps list
        getDetails: async function (name: string): Promise<PumpSwapDetail | null> {

            if (this.getData_busy)
                return null;

            this.getData_busy = true;

            try {

                // Simulate API response with mock data in development, otherwise fetch from the server
                if (isDev()) {

                    let mock: PumpSwapDetail = { name: name, values: [], actions: [], availablePumps: 0, capacityState: 0, requestedPumps: 0, runningPumps: 0, totalPumps: 0 };

                    switch (name) {

                        case "test_4pump":
                            mock.values = [false, false, false, false];
                            mock.actions = ["Reset"];
                            mock.name = "test_4pump";
                            break;
                    };

                    // petit délai pour simuler un vrai fetch (optionnel)
                    await new Promise(r => setTimeout(r, 100));

                    return mock;
                } else {

                    const response = await fetch(`/API/PumpSwaps/${name}`);

                    if (!response.ok) {
                        throw new Error(`HTTP ${response.status}`);
                    }

                    const json = await response.json();

                    return json as PumpSwapDetail;
                }

            } finally {
                this.getData_busy = false;
            }
        },

        uiPreparePumpSwapDetail: function (pumpSwap: PumpSwap, detail: any) {

            let self = this;

            detail.html(''); // Clear previous content
            detail.append(`<div class="title">Propriété de la pumpSwap</div>`);
            detail.append(`<div class="property name"><span>Désignation :</span><span id="pumpSwap-name">${pumpSwap.name}</span></div>`);
            detail.append(`<div class="property values"><span>Valeurs :</span><span id="pumpSwap-values" class='stale'>-</span></div>`);
            detail.append(`<div class="property availablePumps"><span>Nombre de pompes disponibles :</span><span id="pumpSwap-availablePumps" class='stale'>-</span></div>`);
            detail.append(`<div class="property capacityState"><span>Statut de capacité :</span><span id="pumpSwap-capacityState" class='stale'>-</span></div>`);
            detail.append(`<div class="property requestedPumps"><span>Nombre de pompes demandées :</span><span id="pumpSwap-requestedPumps" class='stale'>-</span></div>`);
            detail.append(`<div class="property runningPumps"><span>Nombre de pompes en fonctionnement :</span><span id="pumpSwap-runningPumps" class='stale'>-</span></div>`);
            detail.append(`<div class="property totalPumps"><span>Nombre de pompes total :</span><span id="pumpSwap-totalPumps" class='stale'>-</span></div>`);

            let values = $(".property #pumpSwap-values", detail).empty();
            for (let i = 0; i < pumpSwap.values.length; i++) {

                $(`<div class='value' index='${i}'>${i}</div>`).appendTo(values);
            }

            for (let i = 0; i < pumpSwap.values.length; i++) {

                detail.append(`<div class="title">Propriété du sous-système ${i + 1}</div>`);
                detail.append(`<div class="property properties"><span>Propriétés :</span><span class='pumpSwap-properties' id="pumpSwap-${i}-properties" class='stale'>-</span></div>`);
                /*detail.append(`<div class="property command"><span>Commande :</span><span id="pumpSwap-${i}-command" class='stale'>-</span></div>`);
                detail.append(`<div class="property feedback"><span>Retour de marche :</span><span id="pumpSwap-${i}-feedback" class='stale'>-</span></div>`);
                detail.append(`<div class="property fault"><span>Défaut :</span><span id="pumpSwap-${i}-fault" class='stale'>-</span></div>`);
                detail.append(`<div class="property value"><span>Valeur :</span><span id="pumpSwap-${i}-value" class='stale'>-</span></div>`);*/
                detail.append(`<div class="property runtime"><span>Temps de fonctionnement :</span><span id="pumpSwap-${i}-rt" class='stale'>-</span></div>`);
                detail.append(`<div class="property startCount"><span>Nombre de démarrage :</span><span id="pumpSwap-${i}-sc" class='stale'>-</span></div>`);

                let properties = $(`.property #pumpSwap-${i}-properties`, detail).empty();

                $(`<div class='value' property='command'>Commande</div>`).appendTo(properties);
                $(`<div class='value' property='feedback'>Retour de marche</div>`).appendTo(properties);
                $(`<div class='value' property='fault'>Défaut</div>`).appendTo(properties);
                $(`<div class='value' property='value'>Valeur</div>`).appendTo(properties);
            }

            detail.append(`<div class="title">Actions</div>`);

            let buttons = $("<div class='buttons'>").appendTo(detail);

            for (let i = 0; i < pumpSwap.actions.length; i++) {

                let button = $(`<button action='${pumpSwap.actions[i]}'>${pumpSwap.actions[i]}</button></div>`).appendTo(buttons);
                button.on("click", () => {
                

                    self.setPumpSwapTriggerAction(pumpSwap.name, pumpSwap.actions[i])
                        .then(() => {
                            console.log(`Action ${pumpSwap.actions[i]} triggered on pumpSwap ${pumpSwap.name}`);
                        })
                        .catch((error: string) => {
                            console.error("Error while triggering action in pumpSwap:", error);
                        });
                });

            }

        },

        uiDisplayPumpSwapValueEditor: function (pumpSwap: PumpSwap, container: any) {

            let self = this;

            container.append(`<div class="title">Édition des propriétés persistentes</div>`);
            let form = $("<form></form>").appendTo(container);
            let div = $("<div class='property name'></div>").appendTo(form).append(`<span>Valeur :</span>`);

            // Add submit button
            let buttons = $("<div class='buttons'>").appendTo(form);
            let submitButton = $("<button type='submit'>Mettre à jour</button></div>").appendTo(buttons);

            // Handle form submission
            form.on("submit", (e: any) => {

                e.preventDefault();

                submitButton.attr("disabled", "disabled");

                let newValue: any = { value: null };
                let newDisplayedValue: any = null;

                console.log("Updating pumpSwap with new value:", JSON.stringify(newValue));

                self.setPumpSwapValue(pumpSwap.name, newValue)
                    .then(() => {
                        $(".property #pumpSwap-values", container).html(newDisplayedValue).addClass("stale"); // Update displayed value and add stale class as it may take a moment for the new value to be reflected in the details view
                        submitButton.removeAttr("disabled");
                    })
                    .catch((error: string) => {
                        console.error("Error updating pumpSwap:", error);
                        submitButton.removeAttr("disabled");
                    });

                return false;
            });
        },

        getPumpSwapDataType: function (type: number): string {

            let self = this;

            let dataType = self.dataTypeProperties[type];

            if (dataType !== undefined) {

                return dataType.name;
            }

            return "Unknown";
        },

        uiDisplayPumpSwapDetail: function (pumpSwap: PumpSwapDetail, container: any) {

            let self = this;

            if (container.find("#pumpSwap-name").html() == pumpSwap.name) {

                let values = $(".property #pumpSwap-values", container);
                const elements = $("div.value", values);

                for (let i = 0; i < elements.length; i++) {

                    $(elements[i]).toggleClass("on", pumpSwap.values[i]);
                }

                //container.find("#pumpSwap-values").removeClass("stale").html(self.domGetValues(pumpSwap.values));
                container.find("#pumpSwap-availablePumps").removeClass("stale").html(pumpSwap.availablePumps);
                container.find("#pumpSwap-capacityState").removeClass("stale").html(self.getCapacityString(pumpSwap.capacityState));
                container.find("#pumpSwap-requestedPumps").removeClass("stale").html(pumpSwap.requestedPumps);
                container.find("#pumpSwap-runningPumps").removeClass("stale").html(pumpSwap.runningPumps);
                container.find("#pumpSwap-totalPumps").removeClass("stale").html(pumpSwap.totalPumps);

                for (let i = 0; i < pumpSwap.values.length; i++) {

                    let properties = container.find(`#pumpSwap-${i}-properties`).removeClass("stale");

                    $("div.value[property='command']", properties).toggleClass("on", pumpSwap[i].command);
                    $("div.value[property='feedback']", properties).toggleClass("on", pumpSwap[i].feedback);
                    $("div.value[property='fault']", properties).toggleClass("on", pumpSwap[i].fault);
                    $("div.value[property='value']", properties).toggleClass("on", pumpSwap[i].value);

                    container.find(`#pumpSwap-${i}-rt`).removeClass("stale").html(`${Math.round(pumpSwap[i].runTime / 1000)} s`);
                    container.find(`#pumpSwap-${i}-sc`).removeClass("stale").html(pumpSwap[i].startCount);
                }
            }
        },

        getCapacityString: function (capacity: number): string {

            switch (capacity) {
                case 0: return "Optimal";
                case 1: return "Suffisant";
                case 2: return "Insuffisant";
                case 3: return "Critique";
                default: return "Inconnu";
            }

        },

        setPumpSwapValue: async function (pumpSwapName: string, newValue: any): Promise<void> {

            try {
                const response = await fetch(`/API/PumpSwaps/${pumpSwapName}`, {
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
                console.error("Error in setPumpSwapValue: ", error);
                throw error;
            }
        },

        setPumpSwapTriggerAction: async function (pumpSwapName: string, actionName: string): Promise<void> {

            try {
                const response = await fetch(`/API/PumpSwaps/${pumpSwapName}/actions/${actionName}`, {
                    method: "POST",
                    headers: {
                        "Content-Type": "application/json"
                    },
                    body: JSON.stringify({})
                });
                if (!response.ok) {
                    throw new Error(`HTTP ${response.status}`);
                }
            } catch (error) {
                console.error("Error in setPumpSwapTriggerAction: ", error);
                throw error;
            }
        }
    });
});
