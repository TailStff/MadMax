"use strict";
$(function () {
    function isDev() {
        return window.location.protocol === "file:";
    }
    $.widget("custom.AccumsNavigator", {
        container: null,
        manageAccumsContainer: null,
        getData_busy: false,
        interval: null,
        dataTypeProperties: {},
        options: {},
        _create: function () {
            let self = this;
            self.dataTypeProperties = {
                1: { name: "64 bit double", min: -1.7976931348623157e+308, max: 1.7976931348623157e+308, step: "any" },
                2: { name: "32 bit float", min: -3.4028235e+38, max: 3.4028235e+38, step: "any" },
                3: { name: "64 bit signed integer", min: -9223372036854775808, max: 9223372036854775807, step: 1 },
                4: { name: "64 bit unsigned integer", min: 0, max: 18446744073709551615, step: 1 },
                5: { name: "32 bit signed integer", min: -2147483648, max: 2147483647, step: 1 },
                6: { name: "32 bit unsigned integer", min: 0, max: 4294967295, step: 1 },
                7: { name: "16 bit signed integer", min: -32768, max: 32767, step: 1 },
                8: { name: "16 bit unsigned integer", min: 0, max: 65535, step: 1 },
                9: { name: "8 bit signed integer", min: -128, max: 127, step: 1 },
                10: { name: "8 bit unsigned integer", min: 0, max: 255, step: 1 },
                11: { name: "Boolean", min: 0, max: 1, step: 1 }
            };
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
            self.manageAccumsContainer = $("<div id='AccumsContainer'></div>").appendTo(self.container);
            let list = $("<ul></ul>").appendTo(self.manageAccumsContainer);
            let detail = $("<div id='AccumDetail'></div>").appendTo(self.manageAccumsContainer);
            self.getList()
                .then((json) => {
                if (json === null) {
                    console.warn("getList is busy, please wait.");
                    return;
                }
                json.forEach((Accum) => {
                    let item = $("<li></li>").appendTo(list);
                    item.append($("<span class='icon'></span>"));
                    item.append($("<span></span>").text(Accum.name));
                    item.on("click", () => {
                        self.interval && clearInterval(self.interval);
                        item.addClass("selected").siblings().removeClass("selected");
                        self.uiPrepareAccumDetail(Accum, detail);
                        self.uiDisplayAccumValueEditor(Accum, detail);
                        self.interval = setInterval(() => {
                            self.getDetails(Accum.name)
                                .then((json) => {
                                if (json === null)
                                    return;
                                Accum.value = json.value;
                                self.uiDisplayAccumDetail(json, detail);
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
                        { name: "test_bool", value: false, type: 11 },
                        { name: "test_int", value: 42, type: 7 },
                        { name: "test_float", value: 3.14, type: 2 },
                        { name: "test_double", value: 3.1415, type: 1 }
                    ];
                    await new Promise(r => setTimeout(r, 100));
                    return mock;
                }
                else {
                    const response = await fetch("/API/Accums");
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
                    let mock = { name: name, value: null, type: 0 };
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
                    }
                    ;
                    await new Promise(r => setTimeout(r, 100));
                    return mock;
                }
                else {
                    const response = await fetch(`/API/Accums/${name}`);
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
        uiPrepareAccumDetail: function (Accum, detail) {
            let AccumDataType = this.getAccumDataType(Accum.type);
            detail.html('');
            detail.append(`<div class="title">Propriété de l'Accum</div>`);
            detail.append(`<div class="property name"><span>Désignation :</span><span id="accum-name">${Accum.name}</span></div>`);
            detail.append(`<div class="property value"><span>Valeur :</span><span id="accum-value" class='stale'>-</span></div>`);
            detail.append(`<div class="property type"><span>Type :</span><span id="accum-type">${AccumDataType}</span></div>`);
        },
        uiDisplayAccumValueEditor: function (accum, container) {
            let self = this;
            container.append(`<div class="title">Édition des propriétés persistentes</div>`);
            let form = $("<form></form>").appendTo(container);
            let div = $("<div class='property name'></div>").appendTo(form).append(`<span>Valeur :</span>`);
            let dataType = self.dataTypeProperties[accum.type];
            switch (accum.type) {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                    div.append(`<input id='value' type='number' value='${accum.value}' min='${dataType.min}' max='${dataType.max}' step='${dataType.step}' />`);
                    break;
                case 11:
                    div.append(`<input id='value' type='checkbox' ${accum.value === true ? "checked='checked'" : ""} value='true' />`);
                    break;
            }
            let buttons = $("<div class='buttons'>").appendTo(form);
            let submitButton = $("<button type='submit'>Mettre à jour</button></div>").appendTo(buttons);
            form.on("submit", (e) => {
                e.preventDefault();
                submitButton.attr("disabled", "disabled");
                let newValue = { value: null };
                let newDisplayedValue = null;
                switch (accum.type) {
                    case 1:
                    case 2:
                        newValue.value = parseFloat(form.find("#value").val());
                        newDisplayedValue = newValue.value;
                        break;
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                    case 10:
                        newValue.value = parseInt(form.find("#value").val());
                        newDisplayedValue = newValue.value;
                        break;
                    case 11:
                        newValue.value = form.find("#value").is(":checked");
                        newDisplayedValue = newValue.value ? "true" : "false";
                        break;
                }
                console.log("Updating Accum with new value:", JSON.stringify(newValue));
                self.setAccumValue(accum.name, newValue)
                    .then(() => {
                    $(".property #accum-value", container).html(newDisplayedValue).addClass("stale");
                    submitButton.removeAttr("disabled");
                })
                    .catch((error) => {
                    console.error("Error updating Accum:", error);
                    submitButton.removeAttr("disabled");
                });
                return false;
            });
        },
        getAccumDataType: function (type) {
            let self = this;
            let dataType = self.dataTypeProperties[type];
            if (dataType !== undefined) {
                return dataType.name;
            }
            return "Unknown";
        },
        uiDisplayAccumDetail: function (Accum, container) {
            if (container.find("#accum-name").html() == Accum.name) {
                container.find("#accum-value").removeClass("stale");
                switch (Accum.type) {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                    case 10:
                        container.find("#accum-value").html(Accum.value);
                        break;
                    case 11:
                        container.find("#accum-value").html(Accum.value ? "true" : "false");
                        break;
                }
            }
        },
        setAccumValue: async function (AccumName, newValue) {
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
            }
            catch (error) {
                console.error("Error in setAccumValue: ", error);
                throw error;
            }
        }
    });
});
