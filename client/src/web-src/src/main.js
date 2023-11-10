import "./style.css";
import App from "./App.vue";
import store from "./services/store.js";
import { createApp } from "vue";
import router from "./services/router.js";

import { FontAwesomeIcon } from "@fortawesome/vue-fontawesome";
import fontawesome from "./services/fontawesome.js";

//close python on page close
window.addEventListener("beforeunload", () => {
	eel.close_python();
});

fontawesome.install();

createApp(App).component("font-awesome-icon", FontAwesomeIcon).use(store).use(router).mount("#app");
