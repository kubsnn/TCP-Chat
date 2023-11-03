import "./style.css";
import App from "./App.vue";
import store from "./store.js";
import { createApp } from "vue";
import { createRouter, createWebHistory } from "vue-router";

//close python on page close
window.addEventListener("beforeunload", () => {
	eel.close_python();
});

//fontawesome

/* import the fontawesome core */
import { library } from "@fortawesome/fontawesome-svg-core";

/* import font awesome icon component */
import { FontAwesomeIcon } from "@fortawesome/vue-fontawesome";

/* import specific icons */
import { faPlus, faRightToBracket, faArrowLeft, faSpinner } from "@fortawesome/free-solid-svg-icons";

/* add icons to the library */
library.add(faPlus, faRightToBracket, faArrowLeft, faSpinner);

//routing

import LoginPage from "./views/LoginPage.vue";
import RegisterPage from "./views/RegisterPage.vue";
import Chat from "./views/Chat.vue";
import Home from "./views/Home.vue";

const routes = [
	{
		path: "/",
		name: "home",
		component: Home,
	},
	{
		path: "/login",
		name: "login",
		component: LoginPage,
	},
	{
		path: "/register",
		name: "register",
		component: RegisterPage,
	},
	{
		path: "/chat",
		name: "chat",
		component: Chat,
	},
	{
		path: "/:pathMatch(.*)*",
		redirect: "/",
	},
];

const router = createRouter({
	history: createWebHistory(),
	routes,
});

createApp(App).component("font-awesome-icon", FontAwesomeIcon).use(store).use(router).mount("#app");
