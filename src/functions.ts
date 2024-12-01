import { Zoin } from "./bindings";

export function createWindow({ title, url }: {
    title: string; 
    url: string; 
}) {
    const zoin = new Zoin() 
    zoin.create_window(title, url); 
}
