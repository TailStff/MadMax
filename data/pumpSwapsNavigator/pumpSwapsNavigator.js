"use strict";
$(function () {
    function isDev() {
        return window.location.protocol === "file:";
    }
    $.widget("custom.PumpSwapsNavigator", {
        container: null,
        managePumpSwapsContainer: null,
        getData_busy: false,
        interval: null,
        options: {},
        _create: function () {
            let self = this;
            self.container = self.element;
            self._uiCreateComposant();
            return self;
        },
        _refresh: function () {
        },
        _destroy: function () {
            let self = this;
            self.interval && clearInterval(self.interval);
        },
        _setOptions: function () {
            this._superApply(arguments);
            this._refresh();
        },
        _setOption: function (key, value) {
            this._super(key, value);
        },
        _uiCreateComposant: function () {
            let self = this;
            self.container.html('');
            self.managePumpSwapsContainer = $("<div id='PumpSwapsContainer'></div>").appendTo(self.container);
            let list = $("<ul></ul>").appendTo(self.managePumpSwapsContainer);
            let detail = $("<div id='PumpSwapDetail'></div>").appendTo(self.managePumpSwapsContainer);
            self.getList()
                .then((json) => {
                if (json === null) {
                    console.warn("getList is busy, please wait.");
                    return;
                }
                json.forEach((pumpSwap) => {
                    let item = $("<li></li>").appendTo(list);
                    item.append($("<span class='icon'></span>"));
                    item.append($("<span></span>").text(pumpSwap.name));
                    item.on("click", () => {
                        self.interval && clearInterval(self.interval);
                        item.addClass("selected").siblings().removeClass("selected");
                        self.uiPreparePumpSwapDetail(pumpSwap, detail);
                        self.uiDisplayPumpSwapValueEditor(pumpSwap, detail);
                        self.interval = setInterval(() => {
                            self.getDetails(pumpSwap.name)
                                .then((json) => {
                                if (json === null)
                                    return;
                                pumpSwap.values = json.values;
                                self.uiDisplayPumpSwapDetail(json, detail);
                            })
                                .catch((error) => {
                                console.error("Erreur getDetails:", error);
                            });
                        }, 1000);
                    });
                });
            })
                .catch((error) => {
                console.error("Erreur getData:", error);
            });
        },
        getList: async function () {
            if (this.getData_busy)
                return null;
            this.getData_busy = true;
            try {
                if (isDev()) {
                    const mock = [
                        { name: "test_4pump", values: [false, false, false, false], actions: ["Reset"] }
                    ];
                    await new Promise(r => setTimeout(r, 100));
                    return mock;
                }
                else {
                    const response = await fetch("/API/PumpSwaps");
                    if (!response.ok) {
                        throw new Error(`HTTP ${response.status}`);
                    }
                    const json = await response.json();
                    return json;
                }
            }
            finally {
                this.getData_busy = false;
            }
        },
        getDetails: async function (name) {
            if (this.getData_busy)
                return null;
            this.getData_busy = true;
            try {
                if (isDev()) {
                    let mock = { name: name, values: [], actions: [], availablePumps: 0, capacityState: 0, requestedPumps: 0, runningPumps: 0, totalPumps: 0 };
                    switch (name) {
                        case "test_4pump":
                            mock.values = [false, false, false, false];
                            mock.actions = ["Reset"];
                            mock.name = "test_4pump";
                            break;
                    }
                    ;
                    await new Promise(r => setTimeout(r, 100));
                    return mock;
                }
                else {
                    const response = await fetch(`/API/PumpSwaps/${name}`);
                    if (!response.ok) {
                        throw new Error(`HTTP ${response.status}`);
                    }
                    const json = await response.json();
                    return json;
                }
            }
            finally {
                this.getData_busy = false;
            }
        },
        uiPreparePumpSwapDetail: function (pumpSwap, detail) {
            let self = this;
            detail.html('');
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
                        .catch((error) => {
                        console.error("Error while triggering action in pumpSwap:", error);
                    });
                });
            }
        },
        uiDisplayPumpSwapValueEditor: function (pumpSwap, container) {
            let self = this;
            container.append(`<div class="title">Édition des propriétés persistentes</div>`);
            let form = $("<form></form>").appendTo(container);
            let div = $("<div class='property name'></div>").appendTo(form).append(`<span>Valeur :</span>`);
            let buttons = $("<div class='buttons'>").appendTo(form);
            let submitButton = $("<button type='submit'>Mettre à jour</button></div>").appendTo(buttons);
            form.on("submit", (e) => {
                e.preventDefault();
                submitButton.attr("disabled", "disabled");
                let newValue = { value: null };
                let newDisplayedValue = null;
                console.log("Updating pumpSwap with new value:", JSON.stringify(newValue));
                self.setPumpSwapValue(pumpSwap.name, newValue)
                    .then(() => {
                    $(".property #pumpSwap-values", container).html(newDisplayedValue).addClass("stale");
                    submitButton.removeAttr("disabled");
                })
                    .catch((error) => {
                    console.error("Error updating pumpSwap:", error);
                    submitButton.removeAttr("disabled");
                });
                return false;
            });
        },
        getPumpSwapDataType: function (type) {
            let self = this;
            let dataType = self.dataTypeProperties[type];
            if (dataType !== undefined) {
                return dataType.name;
            }
            return "Unknown";
        },
        uiDisplayPumpSwapDetail: function (pumpSwap, container) {
            let self = this;
            if (container.find("#pumpSwap-name").html() == pumpSwap.name) {
                let values = $(".property #pumpSwap-values", container);
                const elements = $("div.value", values);
                for (let i = 0; i < elements.length; i++) {
                    $(elements[i]).toggleClass("on", pumpSwap.values[i]);
                }
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
        getCapacityString: function (capacity) {
            switch (capacity) {
                case 0: return "Optimal";
                case 1: return "Suffisant";
                case 2: return "Insuffisant";
                case 3: return "Critique";
                default: return "Inconnu";
            }
        },
        setPumpSwapValue: async function (pumpSwapName, newValue) {
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
            }
            catch (error) {
                console.error("Error in setPumpSwapValue: ", error);
                throw error;
            }
        },
        setPumpSwapTriggerAction: async function (pumpSwapName, actionName) {
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
            }
            catch (error) {
                console.error("Error in setPumpSwapTriggerAction: ", error);
                throw error;
            }
        }
    });
});
