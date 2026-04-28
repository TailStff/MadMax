"use strict";
$(function () {
    function isDev() {
        return window.location.protocol === "file:";
    }
    $.widget("custom.VariableNavigator", {
        container: null,
        manageVariablesContainer: null,
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
            self.manageVariablesContainer = $("<div id='VariablesContainer'></div>").appendTo(self.container);
            let list = $("<ul></ul>").appendTo(self.manageVariablesContainer);
            let detail = $("<div id='VariableDetail'></div>").appendTo(self.manageVariablesContainer);
            self.getList()
                .then((json) => {
                if (json === null) {
                    console.warn("getList is busy, please wait.");
                    return;
                }
                json.forEach((variable) => {
                    let item = $("<li></li>").appendTo(list);
                    item.append($("<span class='icon'></span>"));
                    item.append($("<span></span>").text(variable.name));
                    item.on("click", () => {
                        item.addClass("selected").siblings().removeClass("selected");
                        self.uiPrepareVariableDetail(detail);
                        self.uiDisplaySetVariableValue(variable, detail);
                        self.interval && clearInterval(self.interval);
                        self.interval = setInterval(() => {
                            self.getDetails(variable.name)
                                .then((json) => {
                                if (json === null) {
                                    console.warn("getDetails is busy, please wait.");
                                    return;
                                }
                                self.uiDisplayVariableDetail(json, detail);
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
                    const response = await fetch("/API/Variables");
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
                    const response = await fetch(`/API/Variables/${name}`);
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
        uiPrepareVariableDetail: function (detail) {
            detail.html('');
            detail.append(`<div class="title">Propriété de la variable</div>`);
            detail.append(`<div class="property name"><span>Désignation :</span><span id="variable-name">-</span></div>`);
            detail.append(`<div class="property value"><span>Valeur :</span><span id="variable-value">-</span></div>`);
            detail.append(`<div class="property type"><span>Type :</span><span id="variable-type">-</span></div>`);
        },
        uiDisplaySetVariableValue: function (variable, container) {
            let self = this;
            container.append(`<div class="title">Mise à jour de la valeur</div>`);
            let form = $("<form></form>").appendTo(container);
            let div = $("<div class='property name'></div>").appendTo(form).append(`<span>Nouvelle valeur :</span>`);
            switch (variable.type) {
                case 1:
                    div.append(`<input id='value' type='number' value='${variable.value}' step='any'/>`);
                    break;
                case 2:
                    div.append(`<input id='value' type='number' value='${variable.value}' step='any'/>`);
                    break;
                case 3:
                    div.append(`<input id='value' type='number' value='${variable.value}' min='-9223372036854775808' max='9223372036854775807' step='1'/>`);
                    break;
                case 4:
                    div.append(`<input id='value' type='number' value='${variable.value}' min='0' max='18446744073709551615' step='1'/>`);
                    break;
                case 5:
                    div.append(`<input id='value' type='number' value='${variable.value}' min='-2147483648' max='2147483647' step='1'/>`);
                    break;
                case 6:
                    div.append(`<input id='value' type='number' value='${variable.value}' min='0' max='4294967295' step='1'/>`);
                    break;
                case 7:
                    div.append(`<input id='value' type='number' value='${variable.value}' min='-32768' max='32767' step='1'/>`);
                    break;
                case 8:
                    div.append(`<input id='value' type='number' value='${variable.value}' min='0' max='65535' step='1'/>`);
                    break;
                case 9:
                    div.append(`<input id='value' type='number' value='${variable.value}' min='-128' max='127' step='1'/>`);
                    break;
                case 10:
                    div.append(`<input id='value' type='number' value='${variable.value}' min='0' max='255' step='1'/>`);
                    break;
                case 11:
                    div.append(`<input id='value' type='checkbox' ${variable.value === true ? "checked:'checked'" : ""} value='true'/>`);
                    break;
            }
            $(`<div class="button"><button type='submit'>Mettre à jour</button></div>`).appendTo(form);
            form.on("submit", (e) => {
                e.preventDefault();
                let newValue = { value: null };
                switch (variable.type) {
                    case 1:
                    case 2:
                        newValue.value = parseFloat(form.find("#value").val());
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
                        break;
                    case 11:
                        newValue.value = form.find("#value").is(":checked");
                        break;
                }
                self.setVariableValue(variable.name, newValue)
                    .then(() => {
                    console.log("Variable updated successfully");
                })
                    .catch((error) => {
                    console.error("Error updating variable:", error);
                });
                return false;
            });
        },
        uiDisplayVariableDetail: function (variable, container) {
            container.find("#variable-name").html(variable.name);
            switch (variable.type) {
                case 1:
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("64 bit double");
                    break;
                case 2:
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("32 bit float");
                    break;
                case 3:
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("64 bit signed integer");
                    break;
                case 4:
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("64 bit unsigned integer");
                    break;
                case 5:
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("32 bit signed integer");
                    break;
                case 6:
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("32 bit unsigned integer");
                    break;
                case 7:
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("16 bit signed integer");
                    break;
                case 8:
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("16 bit unsigned integer");
                    break;
                case 9:
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("8 bit signed integer");
                    break;
                case 10:
                    container.find("#variable-value").html(variable.value);
                    container.find("#variable-type").html("8 bit unsigned integer");
                    break;
                case 11:
                    container.find("#variable-value").html(variable.value ? "True" : "False");
                    container.find("#variable-type").html("Boolean");
                    break;
            }
        },
        setVariableValue: async function (variableName, newValue) {
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
            }
            catch (error) {
                console.error("Error in setVariableValue: ", error);
                throw error;
            }
        }
    });
});
